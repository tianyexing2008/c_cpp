#include "timer.h"

namespace net
{

muduo::AtomicInt64 CTimer::s_numCreated;

void CTimer::reStart(CTimestamp now)
{
    if(mRepeat)
    {
        mExpiration = addTime(now, mInterval);
    }
    else
    {
        mExpiration = CTimestamp::invalid();
    }
}




}