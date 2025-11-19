#include "pollPoller.h"
#include "logging.h"
#include "channel.h"
#include <poll.h>
/*
struct pollfd {
    int   fd;         // 要监控的文件描述符（-1 表示忽略该元素）
    short events;     // 关注的事件（输入参数，通过位掩码指定）
    short revents;    // 实际发生的事件（输出参数，由内核填充）
};
*/
namespace net
{

CPollPoller::CPollPoller(CEventLoop *loop): CPoller(loop)
{

}

CPollPoller::~CPollPoller() = default;


CTimestamp CPollPoller::poll(int timtoutMs, ChannelList *activeChannels)
{
    int numEvents = ::poll(&*mPollFds.begin(), mPollFds.size(), timtoutMs);
    int saveErrno = errno;
    CTimestamp now(CTimestamp::now());
    if(numEvents > 0)
    {
        LOG_TRACE << numEvents << "events happened\n";
        fillActiveChannels(numEvents, activeChannels);
    }
    else if(numEvents == 0)
    {
        LOG_TRACE << "nothings happened\n";
    }
    else
    {
        if(saveErrno != EINTR)
        {
            errno = saveErrno;
            LOG_SYSERR << "CPollPoller::poll\n";
        }
    }
    return now;
}

void CPollPoller::fillActiveChannels(int numEvents, ChannelList *activeChannels)const
{
    for(PollFdList::const_iterator pfd = mPollFds.begin(); pfd != mPollFds.end() && numEvents > 0; pfd++)
    {
        if(pfd->revents > 0)
        {
            --numEvents;
            ChannelMap::const_iterator ch = mChannels.find(pfd->fd);
            CChannel *channel = ch->second;

            channel->setRevents(pfd->revents);
            activeChannels->push_back(channel);
        }
    }
}

void CPollPoller::updateChannel(CChannel *channel)
{
    assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd() << "events = " << channel->events();
    if(channel->index() < 0)
    {
        //a new one, add to mPollFds
        struct pollfd pfd;
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        mPollFds.push_back(pfd);
        int idx = static_cast<int>(mPollFds.size()) - 1;
        channel->setIndex(idx);
        mChannels[pfd.fd] = channel;
    }
    else //update existing one
    {
        int idx = channel->index();
        struct pollfd &pfd = mPollFds[idx];
        pfd.fd = channel->fd();
        pfd.events = static_cast<short>(channel->events());
        pfd.revents = 0;
        if(channel->isNoneEvent())
        {
            //ignore this fd
            pfd.fd = -channel->fd() - 1;
        }
    }
}

void CPollPoller::removeChannel(CChannel *channel)
{
    CPoller::assertInLoopThread();
    LOG_TRACE << "fd = " << channel->fd();
    int idx = channel->index();
    const struct pollfd& pfd = mPollFds[idx]; (void)pfd;
    size_t n = mChannels.erase(channel->fd());
    if (implicit_cast<size_t>(idx) == mPollFds.size()-1)
    {
        mPollFds.pop_back();
    }
    else
    {
        int channelAtEnd = mPollFds.back().fd;
        iter_swap(mPollFds.begin()+idx, mPollFds.end()-1);
        if (channelAtEnd < 0)
        {
            channelAtEnd = -channelAtEnd-1;
        }
        mChannels[channelAtEnd]->setIndex(idx);
        mPollFds.pop_back();
    }
}

}