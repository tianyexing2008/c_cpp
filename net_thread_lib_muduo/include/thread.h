#ifndef __MUDUO_THREAD_H__
#define __MUDUO_THREAD_H__

#include <string>
#include <functional>
#include <memory>
#include "noncopyable.h"
#include "Atomic.h"

namespace muduo {

class CThread: noncopyable
{
public:
    typedef std::function<void(std::string &)> ThreadFunc;
    explicit CThread(const ThreadFunc &threadFunc, const std::string &name = std::string());
    ~CThread();

    void start();
    void join();
    bool started()const
    {
        return mStarted;
    }

    pid_t tid()const
    {
        return *mTid;
    }

    const std::string& name()const
    {
        return mName;
    }

public:
    static int numCreate()
    {
        return numCreated.get();
    }
private:
    bool mStarted;
    bool mJoined;
    pthread_t mPthreadId;
    std::shared_ptr<pid_t> mTid;
    ThreadFunc mFunc;
    std::string mName;

    static muduo::AtomicInt32 numCreated;
};

namespace CurrentThread
{
extern __thread char t_tidString[32];
extern __thread int t_tidStringLength;

    pid_t tid();
    
    inline const char* tidString()
    {
        return t_tidString;
    }
    inline int tidStringLength()
    {
        return t_tidStringLength;
    }
    
    const char *name();
    bool isMainThread();
}

}
#endif