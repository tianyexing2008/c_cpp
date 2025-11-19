#ifndef __NET_TIMER_QUEUE_H__
#define __NET_TIMER_QUEUE_H__

#include <set>
#include <vector>
#include "mutex.h"
#include "timestamp.h"
#include "callback.h"
#include "channel.h"

namespace net
{
class CEventLoop;
class CTimer;
class CTimerId;

class CTimerQueue: muduo::noncopyable
{
public:
    explicit CTimerQueue(CEventLoop *loop);
    ~CTimerQueue();

    //schedule the callback to be run at given time
    CTimerId addTimer(TimerCallback cb, CTimestamp when, double interval);

    void cancel(CTimerId timeId);

private:
    typedef std::pair<CTimestamp, CTimer*> Entry;
    typedef std::set<Entry> TimerList;
    typedef std::pair<CTimer*, int64_t> ActiveTimer;
    typedef std::set<ActiveTimer> ActiveTimerSet;


    void addTimerInLoop(CTimer *timer);
    void cancelInLoop(CTimerId timerId);
    void handleRead();
    //move out all expired timer
    std::vector<Entry> getExpired(CTimestamp now);
    void reset(const std::vector<Entry> &expired, CTimestamp now);
    bool insert(CTimer *timer);

    CEventLoop *mLoop;
    const int mTimerFd;
    CChannel mTimerFdChannel;
    TimerList mTimers;
    ActiveTimerSet mActiveTimers;
    bool mCallingExpiredTimers;
    ActiveTimerSet mCancelingTimers;
};
















}


#endif