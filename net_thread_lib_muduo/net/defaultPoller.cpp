#include "poller.h"
#include "pollPoller.h"
#include "epollPoller.h"

#include <stdlib.h>

using namespace net;

CPoller* CPoller::newDefaultPoller(CEventLoop* loop)
{
    if (::getenv("MUDUO_USE_POLL"))
    {
        return new CPollPoller(loop);
    }
    else
    {
        return new CEPollPoller(loop);
    }
}
