#ifndef __CUREENT_THREAD_H__
#define __CUREENT_THREAD_H__

///线程局部数据
namespace CurrentThread
{
    extern __thread int tCachedTid;
    extern __thread char tTidString[32];
    extern __thread const char *tThreadName;

    ///线程id缓存，不用每次都调用系统接口
    void cacheTid();

    inline int tid()
    {
        if(tCachedTid == 0)
        {
            cacheTid();
        }

        return tCachedTid;
    }

    inline const char *tidString()
    {
        return tTidString;
    }

    inline const char *name()
    {
        return tThreadName;
    }

    /**
     * @brief 是否是主线程，即main进程
     * 
     * @return true 
     * @return false 
     */
    bool isMainThread();
};

#endif