
#ifndef __NET_EVENTLOOPTHREAD_H__
#define __NET_EVENTLOOPTHREAD_H__

#include "condition.h"
#include "mutex.h"
#include "thread.h"

using namespace muduo;

namespace net
{

class CEventLoop;

class CEventLoopThread : muduo::noncopyable
{
public:
    typedef std::function<void(CEventLoop*)> ThreadInitCallback;

    CEventLoopThread(const ThreadInitCallback& cb = ThreadInitCallback(), const std::string& name = std::string());
    ~CEventLoopThread();
    CEventLoop* startLoop();

private:
    void threadFunc();

    CEventLoop* mLoop;
    bool mExiting;
    CThread mThread;
    CMutex mMutex;
    Condition mCond;
    ThreadInitCallback mCallback;
};

}  // namespace net

#endif  // MUDUO_NET_EVENTLOOPTHREAD_H

