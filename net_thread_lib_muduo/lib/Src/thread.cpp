#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>

#if __FreeBSD__
#include <pthread_np.h>
#else
#include <sys/prctl.h>
#include <linux/unistd.h>
#endif
#include "thread.h"

namespace muduo
{

namespace CurrentThread
{
    __thread const char *t_threadName = "unnamedThread";
    __thread pid_t t_cachedTid = 0;
    __thread char t_tidString[32];
    __thread int t_tidStringLength;
}

}


#if __FreeBSD__
pid_t gettid()
{
    return pthread_getthreadid_np();
}
#else
#if !__GLIBC_PREREQ(2,30)
pid_t gettid()
{
    return static_cast<pid_t>(::syscall(SYS_gettid));
}
#endif
#endif

void afterFork()
{
    muduo::CurrentThread::t_cachedTid = gettid();
    muduo::CurrentThread::t_threadName = "mainThread";
}

class ThreadNameInitializer
{
public:
    ThreadNameInitializer()
    {
        muduo::CurrentThread::t_threadName = "main";
        pthread_atfork(NULL, NULL, &afterFork);
    }
};

ThreadNameInitializer init;

struct ThreadData
{
    typedef muduo::CThread::ThreadFunc ThreadFunc;
    ThreadFunc func_;
    std::string name_;
    std::weak_ptr<pid_t> wkTid_;

    ThreadData(const ThreadFunc &func, const std::string &name, const std::shared_ptr<pid_t> &tid)
    :func_(func), name_(name), wkTid_(tid)
    {

    }

    void runInThread()
    {
        pid_t tid = muduo::CurrentThread::tid();
        std::shared_ptr<pid_t> pTid = wkTid_.lock();
        if(pTid)
        {
            *pTid = tid;
            pTid.reset();
        }

        if(!name_.empty())
        {
#if 1//__FreeBSD__
            // setname_np() costs as much as creating a thread on FreeBSD 13.
            pthread_setname_np(pthread_self(), muduo::CurrentThread::t_threadName);
#else
            ::prctl(PR_SET_NAME, muduo::CurrentThread::t_threadName);
#endif            
        }

        func_(name_);
        muduo::CurrentThread::t_threadName = "finished";
    }
};

void *startThread(void *obj)
{
    ThreadData *data = static_cast<ThreadData*>(obj);
    data->runInThread();

    delete data;
    return NULL;
}



pid_t muduo::CurrentThread::tid()
{
    if(t_cachedTid == 0)
    {
        t_cachedTid = gettid();
        t_tidStringLength = snprintf(t_tidString, sizeof(t_tidString), "%5d ", t_cachedTid);
    }
    return t_cachedTid;
}

const char *muduo::CurrentThread::name()
{
    return t_threadName;
}

bool muduo::CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}

muduo::AtomicInt32 muduo::CThread::numCreated;

muduo::CThread::CThread(const ThreadFunc &func, const std::string &name)
:mStarted(false),
mJoined(false),
mPthreadId(0),
mTid(new pid_t(0)),
mFunc(func),
mName(name)
{
    numCreated.increment();
}

muduo::CThread::~CThread()
{
    if(mStarted && !mJoined)
    {
        pthread_detach(mPthreadId);
    }
}

void muduo::CThread::start()
{
    assert(!mStarted);
    mStarted = true;

    ThreadData *data = new ThreadData(mFunc, mName, mTid);
    if(pthread_create(&mPthreadId, NULL, &startThread, data))
    {
        mStarted = false;
        delete data;
        abort();
    }
}

void muduo::CThread::join()
{
    assert(mStarted);
    assert(!mJoined);
    mJoined = true;
    pthread_join(mPthreadId, NULL);
}