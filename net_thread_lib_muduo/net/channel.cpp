#include "logging.h"
#include "channel.h"
#include "eventLoop.h"

#include <sstream>
#include <poll.h>

namespace net
{

const int CChannel::mNoneEvent = 0;
const int CChannel::mReadEvent = POLLIN | POLLPRI;
const int CChannel::mWriteEvent = POLLOUT;

CChannel::CChannel(CEventLoop* loop, int fd)
:mLoop(loop)
,mFd(fd)
,mEvents(0)
,mRevents(0)
,mIndex(-1)
,mLogHup(true)
,mTied(false)
,mEventHandling(false)
,mAddToLoop(false)
{

}

CChannel::~CChannel()
{
    if(mLoop->isInLoopThread())
    {
    }
}

void CChannel::tie(const std::shared_ptr<void>& obj)
{
    mTie = obj;
    mTied = true;
}

void CChannel::update()
{
    mAddToLoop = true;
    mLoop->updateChannel(this);
}

void CChannel::remove()
{
    mAddToLoop = false;
    mLoop->removeChannel(this);
}

void CChannel::handleEvent(CTimestamp receiveTime)
{
    std::shared_ptr<void> guard;
    if(mTied)
    {
        guard = mTie.lock();
        if(guard != nullptr)
        {
            handleEventWithGuard(receiveTime);
        }
    }
    else
    {
        handleEventWithGuard(receiveTime);
    }
}

void CChannel::handleEventWithGuard(CTimestamp receiveTime)
{
    mEventHandling = true;
    LOG_INFO << reventsToString() << "\n";
    if((mRevents & POLLHUP) && !(mRevents & POLLIN))
    {
        if(mLogHup)
        {
            LOG_WARN << "fd = " << mFd << "channel handle event POLLHUP\n";
        }
        if(mCloseCallback)
        {
            mCloseCallback();
        }
    }

    if(mRevents & POLLNVAL)
    {
        LOG_WARN << "fd = " << mFd << "channel handle event POLLNVAL\n";
    }

    if(mRevents & (POLLERR | POLLNVAL))
    {
        if(mErrorCallback)
        {
            mErrorCallback();
        }
    }

    if(mRevents & (POLLIN | POLLPRI | POLLHUP))
    {
        if(mReadCallback)
        {
            mReadCallback(receiveTime);
        }
    }

    if(mRevents & POLLOUT)
    {
        if(mWriteCallback)
        {
            mWriteCallback();
        }
    }

    mEventHandling = false;
}

std::string CChannel::reventsToString()const
{
    return eventsToString(mFd, mRevents);
}

std::string CChannel::eventsToString()const
{
    return eventsToString(mFd, mRevents);
}

std::string CChannel::eventsToString(int fd, int ev)
{
    std::ostringstream oss;
    oss << fd << ":";
    if(ev & POLLIN)
    {
        oss << "IN ";
    }
    if(ev & POLLPRI)
    {
        oss << "PRI ";
    }
    if(ev & POLLOUT)
    {
        oss << "OUT ";
    }
    if(ev & POLLHUP)
    {
        oss << "HUP ";
    }
    if(ev & POLLRDHUP)
    {
        oss << "RDHUP ";
    }
    if(ev & POLLERR)
    {
        oss << "ERR ";
    }
    if(ev & POLLNVAL)
    {
        oss << "NVAL ";
    }
    return oss.str();
}



















}