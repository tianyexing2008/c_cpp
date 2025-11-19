#include "poller.h"
#include "channel.h"

namespace net
{

CPoller::CPoller(CEventLoop *loop): mOwnerLoop(loop)
{

}

CPoller::~CPoller() = default;

bool CPoller::hasChannel(CChannel *channel)const
{
    assertInLoopThread();
    ChannelMap::const_iterator it = mChannels.find(channel->fd());
    return it != mChannels.end() && it->second == channel;
}

}