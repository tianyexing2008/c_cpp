
#ifndef __THREAD_INTERNAL_H__
#define __THREAD_INTERNAL_H__

#include "Mutex.h"
#include "Semaphore.h"

#if defined WIN32 || defined _WIN64
	typedef void* ThreadHandle;
#elif defined(__linux__)||defined(__mac__)
	typedef pthread_t ThreadHandle;
#elif defined(__TCS__)
	typedef unsigned long ThreadHandle;
#endif

class CThread;
class ThreadManagerInternal;

struct ThreadInternal {

    ThreadHandle                            mHandle;                /**thread句柄*/

    bool		                            mRunning;               /**线程running标记*/
    int32_t		                            mPolicy;                /**线程执行策略*/
    int32_t		                            mPriority;              /**线程优先级*/
    int32_t		                            mStackSize;             /**线程堆栈大小*/
    int32_t		                            mId;                    /**线程Id*/
    char	                                mName[32];              /**线程名*/
    ThreadInternal*                         mPrev;			        /**上一个线程*/
    ThreadInternal*                         mNext;			        /**下一个线程*/
    CSemaphore	                            mSemaphore;	            /**该信号量用来防止同一个对象的线程同时创建多次*/
    uint64_t	                            mExpectedTime;		    /**预计执行时间,0表示不预计*/
    bool	                                mLoop;                  /**线程执行体循环标记*/
    bool	                                mDestroyed;             /**销毁是否标记*/
    bool	                                mDestroyBlock;          /**是否等待线程阻塞退出*/
    ThreadManagerInternal*                  mManager;               /**绑定的线程管理者*/
    CThread*		                        mOwner;                 /**所属CThread*/
    CMutex                                  mMutex;				    /**互斥量*/
};

#endif
