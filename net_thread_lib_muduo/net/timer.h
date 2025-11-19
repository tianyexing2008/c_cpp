#ifndef __NET_TIMER_H__
#define __NET_TIMER_H__

#include "Atomic.h"
#include "callback.h"
#include "timestamp.h"
#include "noncopyable.h"

namespace net
{

class CTimer: muduo::noncopyable
{
public:
    CTimer(TimerCallback cb, CTimestamp when, double interval)
    :mCallback(cb)
    ,mExpiration(when)
    ,mInterval(interval)
    ,mRepeat(interval > 0.0)
    ,mSequence(s_numCreated.incrementAndGet())
    {

    }

    void run()const
    {
        mCallback();
    }

    CTimestamp expiration()const {return mExpiration;}
    bool repeat()const{return mRepeat;}
    int64_t sequence()const{return mSequence;}
    void reStart(CTimestamp now);
    static int64_t numCreated(){s_numCreated.get();}
private:
    const TimerCallback mCallback;
    CTimestamp mExpiration;
    const double mInterval;
    const bool mRepeat;
    const int64_t mSequence;

    static muduo::AtomicInt64 s_numCreated;
};









}
#endif