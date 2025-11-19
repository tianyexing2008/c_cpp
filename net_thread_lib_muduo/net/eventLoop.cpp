
#include "eventLoop.h"
#include "logging.h"
#include "guard.h"
#include "channel.h"
#include "poller.h"
#include "socketsOps.h"
#include "timerQueue.h"

#include <algorithm>
#include <signal.h>
#include <sys/eventfd.h>
#include <unistd.h>

using namespace muduo;
using namespace net;

namespace
{
__thread CEventLoop* t_loopInThisThread = 0;

const int kPollTimeMs = 10000;

int createEventfd()
{
    int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
    if (evtfd < 0)
    {
        LOG_SYSERR << "Failed in eventfd";
        abort();
    }
    return evtfd;
}

#pragma GCC diagnostic ignored "-Wold-style-cast"
class IgnoreSigPipe
{
public:
    IgnoreSigPipe()
    {
        ::signal(SIGPIPE, SIG_IGN);
        // LOG_TRACE << "Ignore SIGPIPE";
    }
};
#pragma GCC diagnostic error "-Wold-style-cast"

IgnoreSigPipe initObj;
}  // namespace

CEventLoop* CEventLoop::getEventLoopOfCurrentThread()
{
    return t_loopInThisThread;
}

CEventLoop::CEventLoop()
  : mLooping(false),
    mQuit(false),
    mEventHandling(false),
    mCallingPendingFunctors(false),
    mIteration(0),
    mThreadId(CurrentThread::tid()),
    mPoller(CPoller::newDefaultPoller(this)),
    mTimerQueue(new CTimerQueue(this)),
    mWakeupFd(createEventfd()),
    mWakeupChannel(new CChannel(this, mWakeupFd)),
    mCurrentActiveChannel(NULL)
{
    LOG_DEBUG << "CEventLoop created " << this << " in thread " << mThreadId;
    if (t_loopInThisThread)
    {
        LOG_FATAL << "Another CEventLoop " << t_loopInThisThread
                    << " exists in this thread " << mThreadId;
    }
    else
    {
        t_loopInThisThread = this;
    }
    mWakeupChannel->setReadCallback(std::bind(&CEventLoop::handleRead, this));
    // we are always reading the wakeupfd
    mWakeupChannel->enableReading();
}

CEventLoop::~CEventLoop()
{
    LOG_DEBUG << "CEventLoop " << this << " of thread " << mThreadId
            << " destructs in thread " << CurrentThread::tid();
    mWakeupChannel->disableAll();
    mWakeupChannel->remove();
    ::close(mWakeupFd);
    t_loopInThisThread = NULL;
}

void CEventLoop::loop()
{
    assertInLoopThread();
    mLooping = true;
    mQuit = false;  // FIXME: what if someone calls quit() before loop() ?
    LOG_TRACE << "CEventLoop " << this << " start looping";

    while (!mQuit)
    {
        mActiveChannels.clear();
        mPollReturnTime = mPoller->poll(kPollTimeMs, &mActiveChannels);
        ++mIteration;
        if (CLogger::logLevel() <= CLogger::TRACE)
        {
            printActiveChannels();
        }
        // TODO sort channel by priority
        mEventHandling = true;
        for (CChannel* channel : mActiveChannels)
        {
            mCurrentActiveChannel = channel;
            mCurrentActiveChannel->handleEvent(mPollReturnTime);
        }
        mCurrentActiveChannel = NULL;
        mEventHandling = false;
        doPendingFunctors();
    }

    LOG_TRACE << "CEventLoop " << this << " stop looping";
    mLooping = false;
}

void CEventLoop::quit()
{
    mQuit = true;
    // There is a chance that loop() just executes while(!mQuit) and exits,
    // then CEventLoop destructs, then we are accessing an invalid object.
    // Can be fixed using mMutex in both places.
    if (!isInLoopThread())
    {
        wakeUp();
    }
}

void CEventLoop::runInLoop(Functor cb)
{
    if (isInLoopThread())
    {
        LOG_INFO << "call directly";
        cb();
    }
    else
    {
        queueInLoop(std::move(cb));
    }
}

void CEventLoop::queueInLoop(Functor cb)
{
    {
        CGuard lock(mMutex);
        mPendingFunctors.push_back(std::move(cb));
    }

    if (!isInLoopThread() || mCallingPendingFunctors)
    {
        wakeUp();
    }
}

size_t CEventLoop::queueSize() const
{
    CGuard lock(mMutex);
    return mPendingFunctors.size();
}

CTimerId CEventLoop::runAt(CTimestamp time, TimerCallback cb)
{
    return mTimerQueue->addTimer(std::move(cb), time, 0.0);
}

CTimerId CEventLoop::runAfter(double delay, TimerCallback cb)
{
    CTimestamp now = CTimestamp::now();
    CTimestamp time(addTime(now, delay));
    return runAt(time, std::move(cb));
}

CTimerId CEventLoop::runEvery(double interval, TimerCallback cb)
{
    CTimestamp now = CTimestamp::now();
    CTimestamp time(addTime(now, interval));
    return mTimerQueue->addTimer(std::move(cb), time, interval);
}

void CEventLoop::cancel(CTimerId timerId)
{
    return mTimerQueue->cancel(timerId);
}

void CEventLoop::updateChannel(CChannel* channel)
{
    assertInLoopThread();
    mPoller->updateChannel(channel);
}

void CEventLoop::removeChannel(CChannel* channel)
{
    assertInLoopThread();
    if (mEventHandling)
    {
        assert(mCurrentActiveChannel == channel ||
            std::find(mActiveChannels.begin(), mActiveChannels.end(), channel) == mActiveChannels.end());
    }
    mPoller->removeChannel(channel);
}

bool CEventLoop::hasChannel(CChannel* channel)
{
    assertInLoopThread();
    return mPoller->hasChannel(channel);
}

void CEventLoop::abortNotInLoopThread()
{
    LOG_FATAL << "CEventLoop::abortNotInLoopThread - CEventLoop " << this
            << " was created in mThreadId = " << mThreadId
            << ", current thread id = " <<  CurrentThread::tid();
}

void CEventLoop::wakeUp()
{
    uint64_t one = 1;
    ssize_t n = sockets::write(mWakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "CEventLoop::wakeup() writes " << n << " bytes instead of 8";
    }
}

void CEventLoop::handleRead()
{
    uint64_t one = 1;
    ssize_t n = sockets::read(mWakeupFd, &one, sizeof one);
    if (n != sizeof one)
    {
        LOG_ERROR << "CEventLoop::handleRead() reads " << n << " bytes instead of 8";
    }
}

void CEventLoop::doPendingFunctors()
{
    std::vector<Functor> functors;
    mCallingPendingFunctors = true;

    {
        CGuard lock(mMutex);
        functors.swap(mPendingFunctors);
    }

    for (const Functor& functor : functors)
    {
        functor();
    }
    mCallingPendingFunctors = false;
}

void CEventLoop::printActiveChannels() const
{
    for (const CChannel* channel : mActiveChannels)
    {
        LOG_TRACE << "{" << channel->reventsToString() << "} ";
    }
}

