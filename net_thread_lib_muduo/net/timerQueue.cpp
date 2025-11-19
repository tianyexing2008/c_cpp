#include "timerQueue.h"
#include "logging.h"
#include "eventLoop.h"
#include "timer.h"
#include "timerId.h"

#include <sys/timerfd.h>
#include <unistd.h>

namespace net
{
namespace detail
{

int createTimerFd()
{
    int timerFd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(timerFd < 0)
    {
        LOG_SYSFATAL << "Failed in timerfd_create\n";
    }
    return timerFd;
}

struct timespec howMuchTimeFormNow(CTimestamp when)
{
    int64_t microseconds = when.microSecondsSinceEpoch() - CTimestamp::now().microSecondsSinceEpoch();
    if(microseconds < 100)
    {
        microseconds = 100;
    }

    struct timespec ts;
    ts.tv_sec = static_cast<time_t>(microseconds / CTimestamp::mMicroSecondPerSecond);
    ts.tv_nsec = static_cast<long>((microseconds % CTimestamp::mMicroSecondPerSecond) * 1000);
    return ts;   
}

void readTimerFd(int timerFd, CTimestamp now)
{
    uint64_t howMany;
    ssize_t n = ::read(timerFd, &howMany, sizeof(howMany));
    LOG_TRACE << "TimerQueue::handleRead() " << howMany << " at " << now.toString();
    if(n != sizeof(howMany))
    {
        LOG_ERROR << "TimerQueue::handleRead() reads " << n << " bytes instead of 8";
    }
}

void resetTimerFd(int timerFd, CTimestamp expiration)
{
    struct itimerspec newValue;
    struct itimerspec oldValue;
    memZero(&newValue, sizeof(newValue));
    memZero(&oldValue, sizeof(oldValue));
    newValue.it_value = howMuchTimeFormNow(expiration);
    int ret = ::timerfd_settime(timerFd, 0, &newValue, &oldValue);
    if(ret)
    {
        LOG_SYSERR << "timerfd_settime()";
    }
}
}


CTimerQueue::CTimerQueue(CEventLoop* loop)
:mLoop(loop)
,mTimerFd(detail::createTimerFd())
,mTimerFdChannel(loop, mTimerFd)
,mTimers()
,mCallingExpiredTimers(false)
{
    mTimerFdChannel.setReadCallback(std::bind(&CTimerQueue::handleRead, this));
    mTimerFdChannel.enableReading();
}


CTimerQueue::~CTimerQueue()
{
    mTimerFdChannel.disableAll();
    mTimerFdChannel.remove();
    ::close(mTimerFd);
    for(const Entry & timer : mTimers)
    {
        delete timer.second;
    }
}

CTimerId CTimerQueue::addTimer(TimerCallback cb, CTimestamp when, double interval)
{
    CTimer *timer = new CTimer(std::move(cb), when, interval);
    mLoop->runInLoop(std::bind(&CTimerQueue::addTimerInLoop, this, timer));
    return CTimerId(timer, timer->sequence());
}

void CTimerQueue::cancel(CTimerId timerId)
{
    mLoop->runInLoop(std::bind(&CTimerQueue::cancelInLoop, this, timerId));
}

void CTimerQueue::addTimerInLoop(CTimer* timer)
{
    mLoop->assertInLoopThread();
    bool earliestChanged = insert(timer);

    if (earliestChanged)
    {
        detail::resetTimerFd(mTimerFd, timer->expiration());
    }
}

void CTimerQueue::cancelInLoop(CTimerId timerId)
{
    mLoop->assertInLoopThread();
    ActiveTimer timer(timerId.mTimer, timerId.mSequence);
    ActiveTimerSet::iterator it = mActiveTimers.find(timer);
    if (it != mActiveTimers.end())
    {
        size_t n = mTimers.erase(Entry(it->first->expiration(), it->first));
        delete it->first; // FIXME: no delete please
        mActiveTimers.erase(it);
    }
    else if (mCallingExpiredTimers)
    {
        mCancelingTimers.insert(timer);
    }
}

void CTimerQueue::handleRead()
{
    mLoop->assertInLoopThread();
    CTimestamp now(CTimestamp::now());
    detail::readTimerFd(mTimerFd, now);

    std::vector<Entry> expired = getExpired(now);

    mCallingExpiredTimers = true;
    mCancelingTimers.clear();
    // safe to callback outside critical section
    for (const Entry& it : expired)
    {
        it.second->run();
    }
    mCallingExpiredTimers = false;

    reset(expired, now);
}

std::vector<CTimerQueue::Entry> CTimerQueue::getExpired(CTimestamp now)
{
    std::vector<Entry> expired;
    Entry sentry(now, reinterpret_cast<CTimer*>(UINTPTR_MAX));
    TimerList::iterator end = mTimers.lower_bound(sentry);
    std::copy(mTimers.begin(), end, back_inserter(expired));
    mTimers.erase(mTimers.begin(), end);

    for (const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        size_t n = mActiveTimers.erase(timer);
    }

    return expired;
}

void CTimerQueue::reset(const std::vector<Entry>& expired, CTimestamp now)
{
    CTimestamp nextExpire;

    for (const Entry& it : expired)
    {
        ActiveTimer timer(it.second, it.second->sequence());
        if (it.second->repeat()
            && mCancelingTimers.find(timer) == mCancelingTimers.end())
        {
            it.second->reStart(now);
            insert(it.second);
        }
        else
        {
            // FIXME move to a free list
            delete it.second; // FIXME: no delete please
        }
    }

    if (!mTimers.empty())
    {
        nextExpire = mTimers.begin()->second->expiration();
    }

    if (nextExpire.valid())
    {
        detail::readTimerFd(mTimerFd, nextExpire);
    }
}

bool CTimerQueue::insert(CTimer* timer)
{
    mLoop->assertInLoopThread();
    bool earliestChanged = false;
    CTimestamp when = timer->expiration();
    TimerList::iterator it = mTimers.begin();
    if (it == mTimers.end() || when < it->first)
    {
        earliestChanged = true;
    }

    {
        std::pair<TimerList::iterator, bool> result = mTimers.insert(Entry(when, timer));
    }
    
    {
        std::pair<ActiveTimerSet::iterator, bool> result = mActiveTimers.insert(ActiveTimer(timer, timer->sequence()));
    }

    return earliestChanged;
}
}