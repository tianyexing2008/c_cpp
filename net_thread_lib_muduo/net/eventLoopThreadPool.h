
#ifndef __NET_EVENTLOOPTHREADPOOL_H__
#define __NET_EVENTLOOPTHREADPOOL_H__

#include "noncopyable.h"
#include "Types.h"

#include <functional>
#include <memory>
#include <vector>
#include <string>

namespace net
{

class CEventLoop;
class CEventLoopThread;

class CEventLoopThreadPool : muduo::noncopyable
{
public:
    typedef std::function<void(CEventLoop*)> ThreadInitCallback;

    CEventLoopThreadPool(CEventLoop* baseLoop, const std::string& nameArg);
    ~CEventLoopThreadPool();
    void setThreadNum(int numThreads) { mNumThreads = numThreads; }
    void start(const ThreadInitCallback& cb = ThreadInitCallback());

    // valid after calling start()
    /// round-robin
    CEventLoop* getNextLoop();

    /// with the same hash code, it will always return the same EventLoop
    CEventLoop* getLoopForHash(size_t hashCode);

    std::vector<CEventLoop*> getAllLoops();

    bool started() const
    { return mStarted; }

    const std::string& name() const
    { return mName; }

private:
    CEventLoop* mBaseLoop;
    std::string mName;
    bool mStarted;
    int mNumThreads;
    int mNext;
    std::vector<std::unique_ptr<CEventLoopThread>> mThreads;
    std::vector<CEventLoop*> mLoops;
};

}  // namespace net

#endif  // MUDUO_NET_EVENTLOOPTHREADPOOL_H
