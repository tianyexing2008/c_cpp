#ifndef __NET_POLL_POLLER_H__
#define __NET_POLL_POLLER_H__

#include "poller.h"
#include <vector>
struct pollfd;

namespace net
{

class CPollPoller: public CPoller
{
public:
    CPollPoller(CEventLoop *loop);
    ~CPollPoller()override;

    CTimestamp poll(int timtoutMs, ChannelList *activeChannels) override;
    void updateChannel(CChannel *channel)override;
    void removeChannel(CChannel *channel)override;

private:
    //把有事件的channel 放到vector里
    void fillActiveChannels(int numEvents, ChannelList *activeChannels)const;
    typedef std::vector<struct pollfd> PollFdList;

    PollFdList mPollFds;
};

}

#endif