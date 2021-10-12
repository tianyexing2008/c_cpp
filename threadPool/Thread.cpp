#include "Thread.h"
#include "ThreadInternal.h"
#include <unistd.h>
#include <string.h>
#include <sys/syscall.h>
#include <sys/resource.h>

// 线程栈大小16KB
#define TSK_DEF_STACK_SIZE 16384

static void *InternalThreadBody(void *pdat)
{
    ThreadInternal *pInternal = (ThreadInternal*)pdat;

	// 设置linux非实时线程的优先级，实际上是设置其nice
	if(pInternal->mPolicy == CThread::policyNormal)
	{
		int priority = -19 + pInternal->mPriority * 40 / (CThread::priorBottom + 1);

		setpriority(PRIO_PROCESS, 0, priority);
	}

	pInternal->mMutex.enter();
	pInternal->mRunning = true;
	pInternal->mId = CThread::getCurrentThreadID();
	pInternal->mMutex.leave();
	// pInternal->mManager->addThread(pInternal);
	printf("ThreadBody Enter name = %s, id = %d, prior = %s%d, stack = %p \n",
		pInternal->mName, pInternal->mId, (pInternal->mPolicy == CThread::policyRealtime) ? "R" : "N", pInternal->mPriority, &pInternal);
	pInternal->mOwner->threadProc();
	printf("ThreadBody leave name = %s, id = %d \n", pInternal->mName, pInternal->mId);
	// pInternal->mManager->removeThread(pInternal);
	pInternal->mLoop = false;

	if (pInternal->mDestroyBlock)
	{
		//tracef("CThread::ThreadProc: post! tid(%d)\n", pInternal->mId);
		pInternal->mSemaphore.post();
	}

	///< mRunning变量基本是单线程使用，可以不用通过锁保护。通过此变量判定线程是否结束，保证此语句后此线程没有语句访问pInternal资源
	pInternal->mRunning = false; // 移到这里设置标志位 保证安全释放此内存
	

	return 0; 
}

CThread::CThread(const char * name, int priority, int policy, int stackSize)
{
    mInternal =  new ThreadInternal;
    
    mInternal->mOwner = this;
    mInternal->mPriority = priority;
    mInternal->mPolicy = policy;
    mInternal->mStackSize = stackSize;
    mInternal->mId = -1;
    mInternal->mRunning = false;
    mInternal->mLoop = false;
    mInternal->mDestroyed = false;
    mInternal->mDestroyBlock = true;
    mInternal->mExpectedTime = 0;
    mInternal->mPrev = nullptr;
    mInternal->mNext = nullptr;
    if(strlen(name) > 0)
    {
        snprintf(mInternal->mName, sizeof(mInternal->mName), "%s", name);
    }
    else
    {
        snprintf(mInternal->mName, sizeof(mInternal->mName), "%s", "noName");
    }
}

CThread::~CThread()
{

}

bool CThread::createThread()
{
    mInternal->mMutex.enter();
    if(mInternal->mLoop) {
        mInternal->mMutex.leave();
        printf("thread %s is still running\n", mInternal->mName);
        return false;
    }

    /**
     * 加上mRunning判断可以避免一下问题:在DestroyThread֮之后线程体还未退出RemoveThread也没有调用
     * 但mLoop为false，CreateThread再次进入，会再次创建线程体并调用AddThread;相当于AddThread
     * 调用了两次,从而引起线程管理链表死链,进而访问链表时出现死循环
     */
    while(mInternal->mRunning) {
        /** 解锁，让正在退出的线程无障碍退出**/
        mInternal->mMutex.leave();
        sleep(1);
        mInternal->mMutex.enter();
    }

    /**
     * 再次判断mLoop 是为了解决多个线程在同一个线程对象上创建线程体引起的竞争
     */
    if(mInternal->mLoop) {
        mInternal->mMutex.leave();
        printf("thread %s is still running\n", mInternal->mName);
        return false;
    }

    /**获取创建权,设置loop标记*/
    mInternal->mLoop = true;

    /**需要再次初始化这些状态*/
    mInternal->mId = -1;
    mInternal->mDestroyed = false;
    mInternal->mDestroyBlock = true;
    mInternal->mExpectedTime = 0;

    /**make pretty sure that the count of mSemaphore has been set to 0*/
    while(mInternal->mSemaphore.tryPend()>=0) continue;

    int32_t stkSize = mInternal->mStackSize < TSK_DEF_STACK_SIZE? TSK_DEF_STACK_SIZE:mInternal->mStackSize;

    pthread_attr_t		attr;
    int32_t ret = pthread_attr_init(&attr);
    if(ret != 0) {
        mInternal->mMutex.leave();
        printf("pthread attribute setConfig failed,errno:%d\n", ret);
        return false;
    }

    if (mInternal->mPolicy == policyRealtime) {
        ret = pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED);
        if(ret != 0) {
            mInternal->mMutex.leave();
            printf("pthread attr setinheritsched PTHREAD_EXPLICIT_SCHED failed, errno=%d\n", ret);
            return false;
        }

        ret = pthread_attr_setschedpolicy(&attr, SCHED_FIFO);
        if(ret != 0) {
            mInternal->mMutex.leave();
            printf("pthread attr set SCHED_FIFO failed, errno=%d\n", ret);
            return false;
        }

        /** 设置linux实时线程优先级*/
        struct sched_param param;
        int32_t priorMin = sched_get_priority_min(SCHED_FIFO);
        int32_t priorMax = sched_get_priority_max(SCHED_FIFO);

        param.sched_priority = priorMax - (mInternal->mPriority - CThread::priorTop) * (priorMax - priorMin)/(CThread::priorBottom - CThread::priorTop);
        ret = pthread_attr_setschedparam(&attr, &param);

        if(ret != 0) {
            mInternal->mMutex.leave();
            printf("pthread attr pthread_attr_setschedparam(sched priority=%d) failed, errno=%d\n", param.sched_priority,ret);
            return false;
        }
    } else {
        if(mInternal->mPolicy != policyNormal) {
            mInternal->mMutex.leave();
            printf("CThread::CreateThread policy isn't set properly, policy = %d\n", mInternal->mPolicy);
        }

        ret = pthread_attr_setschedpolicy(&attr, SCHED_OTHER);
        if(ret != 0) {
            mInternal->mMutex.leave();
            printf("pthread attr pthread_attr_setschedpolicy(SCHED_OTHER) failed, errno=%d\n", ret );
            return false;
        }
    }

    ret = pthread_create(&mInternal->mHandle, &attr,(void* (*)(void *))InternalThreadBody, (void *)mInternal);
    if(ret != 0) {
        mInternal->mMutex.leave();
        printf("pthread create failed, errno=%d\n",ret);
        return false;
    }

    ret = pthread_detach(mInternal->mHandle);
    if(ret != 0) {
        mInternal->mMutex.leave();
        printf("pthread detach failed, errno=%d\n",ret);
        return false;
    }

    ret = pthread_attr_destroy(&attr);
    if(ret != 0) {
        mInternal->mMutex.leave();
        printf("pthread attr destroy failed, errno=%d\n",ret);
        return false;
    }

    bool succ = (ret == 0);
    mInternal->mRunning = (succ && mInternal->mLoop);
    mInternal->mMutex.leave();
    return succ;
}

bool CThread::isThreadOver()
{
	return !mInternal->mRunning && !mInternal->mLoop;
}

bool CThread::destroyThread()
{
	mInternal->mMutex.enter();

	if(mInternal->mDestroyed)
	{
		mInternal->mMutex.leave();

		printf("CThread::DestroyThread() thread '%s' has been destroyed!\n", mInternal->mName);

		return false;
	}
	
	//避免没有createThread就直接destoryThread导致永久阻塞
	if(isThreadOver())
	{
		mInternal->mMutex.leave();

		printf("CThread::DestroyThread() thread '%s' has exited!\n", mInternal->mName);

		return false;
	}	

	mInternal->mLoop = false;
	mInternal->mDestroyed = true;

	mInternal->mMutex.leave();

	// 等待线程结束,自己关自己的时候才采用非阻塞式
	if (mInternal->mId == getCurrentThreadID())
	{
		mInternal->mDestroyBlock = false;
	}
	else
	{
		while(mInternal->mRunning){
			if(-1 != mInternal->mSemaphore.pend(50) || !mInternal->mDestroyed) break;
		}
	}

	return true;
}

int CThread::getCurrentThreadID()
{
#if defined WIN32 || defined _WIN64
	return (int)GetCurrentThreadId();
#elif defined(__mac__)
	return pthread_mach_thread_np(pthread_self());
#elif defined(__linux__) && !defined __native_client__
#ifndef SYS_gettid
#define SYS_gettid __NR_gettid 
#endif
	return (int)syscall(SYS_gettid);
#elif defined(__TCS__)
	int tid;
	unsigned long ret;
	ret = t_getreg(0, TNR_OWNERID, (unsigned long *)&tid);
	//DAHUA_ASSERT(ret == 0);
	if(ret != 0 )
	{
		printf_log("t_getreg failed\n");
		return -1;
	}
	return tid;
#elif defined __native_client__
	return (int)pthread_self();
#endif
}

bool CThread::looping()const
{
    return mInternal->mLoop;
}