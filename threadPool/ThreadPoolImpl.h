#ifndef __THREAD_POOLIMPL_H__
#define __THREAD_POOLIMPL_H__

#include "ThreadPool.h"

///前置声明
class Internal; 
class CThreadPoolImpl: public IThreadPool
{
public:
    CThreadPoolImpl(/* args */);

    ~CThreadPoolImpl();

    /// @brief 线程池初始化，创建n个线程
    /// param[in] workThread 线程个数
    /// taskNum 任务个数
    virtual bool initialize(int workThread, int taskNum = 1024);

    virtual bool deInitialize();

    //添加任务
    virtual bool postTask(IThreadTask *task);

private:
    Internal *mInternal;
};


#endif