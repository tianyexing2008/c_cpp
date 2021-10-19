#ifndef __UNI_THREAD_H__
#define __UNI_THREAD_H__

#include <boost/function.hpp>
#include <boost/noncopyable.hpp>
#include <pthread.h>


class CThread: public boost::noncopyable
{
public:
    typedef boost::function<void ()> threadProc;
    explicit CThread(const threadProc& proc, std::string threaName);
    ~CThread();

    void createThread();

    bool started()const 
    {
        return mStarted;
    }

    pid_t tid() const
    {
        return mTid;
    }

    const std::string& name()const
    {
        return mName;
    }

    int join();
private:
    static void* startThread(void* arg);

    void runInThread();
private:
    bool       mStarted;
    pthread_t  mPthreadId;
    pid_t      mTid;
    threadProc funcProc;
    std::string     mName;
};

#endif
