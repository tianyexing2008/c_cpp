#ifndef __CURRENT_THREAD_H__
#define __CURRENT_THREAD_H__

#include <unistd.h>
#include <sys/syscall.h>
namespace CurrentThread
{
    __thread pid_t sThreadId = 0;

    inline pid_t gettid()
    {
        if(sThreadId == 0)
        {
            sThreadId = static_cast<pid_t>(::syscall(SYS_gettid));
        }
        return sThreadId;
    }

    // inline bool isMainThread()
    // {
    //     return gettid() == ::getpid();
    // }
};
#endif
