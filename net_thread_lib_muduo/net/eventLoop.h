#ifndef __NET_EVENTLOOP_H__
#define __NET_EVENTLOOP_H__

#include <atomic>
#include <functional>
#include <vector>

#include "mutex.h"
#include "thread.h"
#include "noncopyable.h"
#include "timerId.h"
#include "callback.h"

namespace net
{

class CChannel;
class CPoller;
class CTimerQueue;

class CEventLoop: muduo::noncopyable
{
public:
    typedef std::function<void ()> Functor;
    CEventLoop();
    ~CEventLoop();

    //loop forever
    void loop();

    //quit loop
    void quit();

    CTimestamp pollReturnTime()const
    {
        return mPollReturnTime;
    }

    int64_t iteration()const
    {
        return mIteration;
    }

    //Run callback immediately in the loop thread
    void runInLoop(Functor cb);

    //Queue callback in the loop thread
    void queueInLoop(Functor cb);

    size_t queueSize()const;

    //Run callback at 'time'
    CTimerId runAt(CTimestamp time, TimerCallback cb);

    //Runs callback after delay second
    CTimerId runAfter(double delay, TimerCallback cb);

    //Runs callback every interval second
    CTimerId runEvery(double interval, TimerCallback cb);

    //Cancle the timer
    void cancel(CTimerId);

    void wakeUp();
    void updateChannel(CChannel *channel);
    void removeChannel(CChannel *channel);
    bool hasChannel(CChannel *channel);
    void assertInLoopThread()
    {
        if(!isInLoopThread())
        {
            abortNotInLoopThread();
        }
    }
    
    bool isInLoopThread()const
    {
        return mThreadId == muduo::CurrentThread::tid();
    }

    bool eventHandling()const
    {
        return mEventHandling;
    }

    /*void setContext(const boost::any &context)
    {
        mContext = context;
    }

    const boost::any& getContext()const
    {
        return mContext;
    }

    boost::any *getMutableContext()
    {
        return &mContext;
    }*/

    static CEventLoop *getEventLoopOfCurrentThread();
private:
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();
    void printActiveChannels()const;
    typedef std::vector<CChannel*> ChannelList;

    bool mLooping;
    std::atomic<bool> mQuit;
    bool mEventHandling;
    bool mCallingPendingFunctors;
    int64_t mIteration;
    const pid_t mThreadId;
    CTimestamp mPollReturnTime;
    std::unique_ptr<CPoller> mPoller;
    std::unique_ptr<CTimerQueue> mTimerQueue;
    int mWakeupFd;
    std::unique_ptr<CChannel> mWakeupChannel;
    //boost::any mContext;
    ChannelList mActiveChannels;
    CChannel *mCurrentActiveChannel;
    mutable CMutex mMutex;
    std::vector<Functor> mPendingFunctors;
};  

}

#endif