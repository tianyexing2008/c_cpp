#ifndef __NET_EPOLL_POLLER_H__
#define __NET_EPOLL_POLLER_H__

#include "poller.h"
#include <vector>
struct epoll_event;

namespace net
{

class CEPollPoller: public CPoller
{
public:
    CEPollPoller(CEventLoop *loop);
    ~CEPollPoller()override;

    CTimestamp poll(int timtoutMs, ChannelList *activeChannels) override;
    void updateChannel(CChannel *channel)override;
    void removeChannel(CChannel *channel)override;

private:
    static const int kInitEventListSize = 16;
    static const char* operationToString(int op);

    //把有事件的channel 放到vector里
    void fillActiveChannels(int numEvents, ChannelList *activeChannels)const;
    void update(int operation, CChannel* channel);
    typedef std::vector<struct epoll_event> PollFdList;

    int mEpollFd;
    PollFdList mEventsVector;
};

}

#endif