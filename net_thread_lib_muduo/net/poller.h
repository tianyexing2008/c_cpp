#ifndef __NET_POLLER_H__
#define __NET_POLLER_H__

#include "noncopyable.h"
#include "timestamp.h"
#include "eventLoop.h"
#include <vector>
#include <map>

namespace net
{

class CChannel;

class CPoller: muduo::noncopyable
{
public:
    typedef std::vector<CChannel*> ChannelList;
    CPoller(CEventLoop *loop);
    virtual ~CPoller();

    virtual CTimestamp poll(int timtoutMs, ChannelList *activeChannels) = 0;
    virtual void updateChannel(CChannel *channel) = 0;
    virtual void removeChannel(CChannel *channel) = 0;
    virtual bool hasChannel(CChannel *channel)const;

    static CPoller *newDefaultPoller(CEventLoop *loop);

    void assertInLoopThread()const
    {
        mOwnerLoop->assertInLoopThread();
    }

protected:
    typedef std::map<int, CChannel*> ChannelMap;
    ChannelMap mChannels;

private:
    CEventLoop *mOwnerLoop;
};

}

#endif