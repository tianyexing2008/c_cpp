#include <assert.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <errno.h>
#include "Thread.h"
#include "CurrentThread.h"

namespace detail
{
    pid_t gettid()
    {
        return static_cast<pid_t>(::syscall(SYS_gettid));
    }
};


/*****************************
 * namespace CurrentThread
 * **************************/
namespace CurrentThread
{
    __thread int tCachedTid = 0;
    __thread char tTidString[32];
    __thread const char *tThreadName = "unknow";

};

void CurrentThread::cacheTid()
{
    ///第一次调用时会调用系统函数
    if(tCachedTid == 0)
    {
        tCachedTid = detail::gettid();
        snprintf(tTidString, sizeof(tTidString), "%5d ", tCachedTid);
    }
}

bool CurrentThread::isMainThread()
{
    return tid() == ::getpid();
}


/*****************************
 * CThread
 * **************************/
CThread::CThread(const threadProc& proc, std::string threaName):
mStarted(false),
mPthreadId(-1),
funcProc(proc),
mName(threaName)
{

}

CThread::~CThread()
{

}

void CThread::createThread()
{
    assert(!mStarted);
    mStarted = true;

    errno = pthread_create(&mPthreadId, NULL, startThread, this);
    if(errno < 0)
    {

    }
}

void* CThread::startThread(void* arg)
{
    CThread *thread = static_cast<CThread*>(arg);
    thread->runInThread();
    return nullptr;
}

void CThread::runInThread()
{
    mTid = CurrentThread::tid();
    if(funcProc)
    {
        funcProc();
    }
}

int CThread::join()
{
    assert(mStarted);
    return pthread_join(mPthreadId, nullptr);
}
