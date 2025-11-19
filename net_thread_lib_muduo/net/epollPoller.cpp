#include "epollPoller.h"
#include "logging.h"
#include "channel.h"

#include <assert.h>
#include <errno.h>
#include <poll.h>
#include <sys/epoll.h>
#include <unistd.h>

using namespace net;

// On Linux, the constants of poll(2) and epoll(4)
// are expected to be the same.

namespace
{
    const int kNew = -1;
    const int kAdded = 1;
    const int kDeleted = 2;
}

CEPollPoller::CEPollPoller(CEventLoop* loop)
:CPoller(loop)
,mEpollFd(::epoll_create1(EPOLL_CLOEXEC))
,mEventsVector(kInitEventListSize)
{
    if (mEpollFd < 0)
    {
        LOG_SYSFATAL << "CEPollPoller::CEPollPoller";
    }
}

CEPollPoller::~CEPollPoller()
{
    ::close(mEpollFd);
}

CTimestamp CEPollPoller::poll(int timeoutMs, ChannelList* activeChannels)
{
    LOG_TRACE << "fd total count " << mChannels.size();
    int numEvents = ::epoll_wait(mEpollFd,
                                &*mEventsVector.begin(),
                                static_cast<int>(mEventsVector.size()),
                                timeoutMs);
    int savedErrno = errno;
    CTimestamp now(CTimestamp::now());
    if (numEvents > 0)
    {
        LOG_TRACE << numEvents << " events happened";
        fillActiveChannels(numEvents, activeChannels);
        if (implicit_cast<size_t>(numEvents) == mEventsVector.size())
        {
            mEventsVector.resize(mEventsVector.size()*2);
        }
    }
    else if (numEvents == 0)
    {
        LOG_TRACE << "nothing happened";
    }
    else
    {
        // error happens, log uncommon ones
        if (savedErrno != EINTR)
        {
            errno = savedErrno;
            LOG_SYSERR << "CEPollPoller::poll()";
        }
    }
    return now;
}

void CEPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const
{
    for (int i = 0; i < numEvents; ++i)
    {
        CChannel* channel = static_cast<CChannel*>(mEventsVector[i].data.ptr);
#ifndef NDEBUG
        int fd = channel->fd();
        ChannelMap::const_iterator it = mChannels.find(fd);
#endif
        channel->setRevents(mEventsVector[i].events);
        activeChannels->push_back(channel);
    }
}

void CEPollPoller::updateChannel(CChannel* channel)
{
    CPoller::assertInLoopThread();
    const int index = channel->index();
    LOG_TRACE << "fd = " << channel->fd()
    << " events = " << channel->events() << " index = " << index;
    if (index == kNew || index == kDeleted)
    {
        // a new one, add with EPOLL_CTL_ADD
        int fd = channel->fd();
        if (index == kNew)
        {
            mChannels[fd] = channel;
        }
        else // index == kDeleted
        {
        }

        channel->setIndex(kAdded);
        update(EPOLL_CTL_ADD, channel);
    }
    else
    {
        // update existing one with EPOLL_CTL_MOD/DEL
        int fd = channel->fd();
        (void)fd;
        if (channel->isNoneEvent())
        {
            update(EPOLL_CTL_DEL, channel);
            channel->setIndex(kDeleted);
        }
        else
        {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void CEPollPoller::removeChannel(CChannel* channel)
{
    CPoller::assertInLoopThread();
    int fd = channel->fd();
    LOG_TRACE << "fd = " << fd;
    int index = channel->index();
    size_t n = mChannels.erase(fd);
    (void)n;

    if (index == kAdded)
    {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->setIndex(kNew);
}

void CEPollPoller::update(int operation, CChannel* channel)
{
    struct epoll_event event;
    memZero(&event, sizeof event);
    event.events = channel->events();
    event.data.ptr = channel;
    int fd = channel->fd();
    LOG_TRACE << "epoll_ctl op = " << operationToString(operation)
    << " fd = " << fd << " event = { " << channel->eventsToString() << " }";
    if (::epoll_ctl(mEpollFd, operation, fd, &event) < 0)
    {
        if (operation == EPOLL_CTL_DEL)
        {
            LOG_SYSERR << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
        }
        else
        {
            LOG_SYSFATAL << "epoll_ctl op =" << operationToString(operation) << " fd =" << fd;
        }
    }
}

const char* CEPollPoller::operationToString(int op)
{
    switch (op)
    {
        case EPOLL_CTL_ADD:
            return "ADD";
        case EPOLL_CTL_DEL:
            return "DEL";
        case EPOLL_CTL_MOD:
            return "MOD";
        default:
            return "Unknown Operation";
    }
}
