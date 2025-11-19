#ifndef __NET_TIMER_ID_H__
#define __NET_TIMER_ID_H__

#include <inttypes.h>

namespace net
{

class CTimer;

class CTimerId
{
public:
    CTimerId(): mTimer(nullptr), mSequence(0)
    {

    }

    CTimerId(CTimer *timer, int64_t seq): mTimer(timer), mSequence(seq)
    {

    }

    friend class CTimerQueue;
private:
    CTimer *mTimer;
    int64_t mSequence;
};


}
#endif