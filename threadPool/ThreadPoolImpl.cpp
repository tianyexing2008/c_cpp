#include <mutex>
#include <queue>
#include <atomic>
#include <chrono>
#include <condition_variable>
#include "ThreadPoolImpl.h"
#include "Thread.h"
#include "Log.h"

///线程池内部数据
class Internal
{
    friend class CThreadPoolImpl;
public:
    Internal()
    {

    }
    ~Internal(){}

    /**
     * @brief 线程执行函数
     * 
     * @param thread 
     */
    void threadProc(CThreadLite &thread);

private:
    bool                            mInit;      //初始化标志
    std::atomic<int32_t>            mWorkNum;   //工作线程娄
    std::atomic<int32_t>            mIdleNum;   //空闲线程数
    int32_t                         mQueueSize; //任务队列大小
    std::mutex                      mMutex;     //互斥锁
    std::condition_variable         mCond;      //条件变量,用于唤醒等待的线程
    std::vector<CThreadLite*>       mWorkTheads; //创建的线程
    std::queue<IThreadTask*>        mThreadTasks;//任务队列
};

void Internal::threadProc(CThreadLite &thread)
{
    while(thread.looping())
    {
        IThreadTask *task = nullptr;
        ///临界区
        {
            ///条件变量等待，阻塞100毫秒
            infof("cond wait_for start!\n");
            std::unique_lock<std::mutex> locker(mMutex);
            mCond.wait_for(locker, std::chrono::milliseconds(100));
            infof("cond wait_for end!\n");
            if(!mThreadTasks.empty())
            {
                task = mThreadTasks.front();
                mThreadTasks.pop();
                mIdleNum--;
            }
        }

        if(task != nullptr)
        {
            task->trigger();
            delete task;
            mIdleNum++;
        }
    }
}

IThreadPool* IThreadPool::create()
{
    CThreadPoolImpl *threadPool = new CThreadPoolImpl();
    return threadPool;
}

CThreadPoolImpl::CThreadPoolImpl()
{
    mInternal = new Internal();
}

CThreadPoolImpl::~CThreadPoolImpl()
{
    delete mInternal;
    mInternal = nullptr;
}

bool CThreadPoolImpl::initialize(int workThread, int taskNum)
{
    ///是否已经初始化
    if(mInternal->mInit)
    {
        return true;
    }

    workThread = workThread < 1 ? 1 : workThread;

    ///创建n个线程
    char threadName[128] = {0};
    for(int i = 0; i < workThread; i++)
    {
        snprintf(threadName, sizeof(threadName), "treadpool[tid-%d]", i);
        CThreadLite *threadLite = new CThreadLite(CThreadLite::ThreadProc(&Internal::threadProc, mInternal), threadName);

        ///创建线程，执行函数体
        if(threadLite->createThread())
        {
            mInternal->mWorkTheads.push_back(threadLite);
        }
    }

    ///初始化工作线程数、空闲线程数
    mInternal->mInit = true;
    mInternal->mWorkNum = workThread;
    mInternal->mIdleNum = workThread;
    mInternal->mQueueSize = taskNum;

    return true;
}

bool CThreadPoolImpl::deInitialize()
{
    if (!mInternal->mInit) 
    {
        return true;
    }

    {
        std::lock_guard<std::mutex> locker(mInternal->mMutex);
        while (!mInternal->mThreadTasks.empty()) 
        {
            IThreadTask *task = mInternal->mThreadTasks.front();
            mInternal->mThreadTasks.pop();
            delete task;
        }
    }

    ///销毁线程，释放资源
    for (uint32_t i = 0; i < mInternal->mWorkTheads.size(); i++) 
    {
        mInternal->mWorkTheads[i]->destroyThread();
        delete mInternal->mWorkTheads[i];
    }
    
    mInternal->mInit = false;
    mInternal->mWorkTheads.clear();

    return true;
}

///添加任务
bool CThreadPoolImpl::postTask(IThreadTask *task)
{
    std::lock_guard<std::mutex> locker(mInternal->mMutex);
    if(mInternal->mThreadTasks.size() > mInternal->mQueueSize)
    {
        errorf("too many task!\n");
        return false;
    }

    ///添加一个任务，唤醒一个等待的线程
    mInternal->mThreadTasks.push(task);
    mInternal->mCond.notify_one();

    return true;
}

