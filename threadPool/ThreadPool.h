#ifndef __THREAD_POOL_H__
#define __THREAD_POOL_H__

#include "ThreadTask.h"

class IThreadPool
{
public:
    static IThreadPool* create();

    static void destroy(IThreadPool *threadpool);

public:
    virtual ~IThreadPool(){}

    /// @brief 
    /// param[in] workThread 线程个数
    /// taskNum 任务个数
    virtual bool initialize(int workThread, int taskNum = 1024) = 0;

    virtual bool deInitialize() = 0;

    //添加任务
    virtual bool postTask(IThreadTask *task) = 0;
};

#endif
