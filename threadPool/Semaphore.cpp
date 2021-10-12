#include <semaphore.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#include <assert.h>
#include "Semaphore.h"

struct SemaphoreInternal
{
	pthread_condattr_t		mCondattr;
	pthread_mutex_t			mtx;
	pthread_cond_t			mCv;
	int						mCount;
};

CSemaphore::CSemaphore(int initialCount)
{
	mInternal = new SemaphoreInternal;

	int ret = 0;
	ret = pthread_condattr_init(&mInternal->mCondattr); 
	assert(ret == 0);
	ret = pthread_condattr_setclock(&mInternal->mCondattr, CLOCK_MONOTONIC); 
	assert(ret == 0);
	/* initialize a condition variable to its default value */
	ret = pthread_cond_init(&mInternal->mCv, &mInternal->mCondattr);
	assert(ret == 0);
	/* initialize a condition variable */
	ret = pthread_mutex_init(&mInternal->mtx, NULL);
	assert(ret == 0);
	(void)ret;	// 消除变量未使用编译警告
	assert(initialCount >= 0);
	mInternal->mCount = initialCount;
}

CSemaphore::~CSemaphore()
{
	int ret = pthread_mutex_destroy(&mInternal->mtx);
	assert(ret == 0);
	(void)ret;	// 消除变量未使用编译警告
	ret = pthread_cond_destroy(&mInternal->mCv);
	assert(ret == 0);
	delete mInternal;
	mInternal = NULL;
}

int CSemaphore::pend()
{
	int ret = 0;
	pthread_mutex_lock(&mInternal->mtx);
	while (mInternal->mCount == 0 && 0 == ret)
		ret = pthread_cond_wait(&mInternal->mCv, &mInternal->mtx);	

	if(ret!=0){
		pthread_mutex_unlock(&mInternal->mtx);
		return -1;
	}
	mInternal->mCount = mInternal->mCount -1;
	int finalcnt = mInternal->mCount;
	ret = pthread_mutex_unlock(&mInternal->mtx);
	return (ret == 0)? finalcnt : -1;
}

int CSemaphore::post()
{
	int ret = 0;
	pthread_mutex_lock(&mInternal->mtx);
	/*即使多signal，如果没有wait等待，也只是会无效而浪费掉，
	mCount永远可以正确的计算signal也即post的次数，
	signal可能会有很多次造成浪费-即使没有wait来等待就会浪费。*/
	ret=pthread_cond_signal(&mInternal->mCv);
	mInternal->mCount = mInternal->mCount +1;
	int finalcnt = mInternal->mCount;
	ret = pthread_mutex_unlock(&mInternal->mtx);
	return (ret == 0)? finalcnt : -1;

}

int CSemaphore::pend(uint32 timeout)
{
	int err = 0;
	struct timespec to={0},now={0};
	if(clock_gettime(CLOCK_MONOTONIC, &now) == -1)
	{
		printf("clock_gettime failed,errno = %d\n", errno);
		return -1;
	}

	to.tv_sec=now.tv_sec + (now.tv_nsec +(timeout%1000L)*1000000L)/1000000000L + timeout/1000L;
	to.tv_nsec=(now.tv_nsec +(timeout%1000L)*1000000L)%1000000000L;	
	
	pthread_mutex_lock(&mInternal->mtx);
	while (mInternal->mCount == 0 && 0 == err){//超时时err不为0，此时可以自动在条件判断时退出循环
		err = pthread_cond_timedwait(&mInternal->mCv, &mInternal->mtx,&to);
	}

	if(0 == err)
		--mInternal->mCount;

	int finalcnt=mInternal->mCount;
	pthread_mutex_unlock(&mInternal->mtx);
	return (err == 0)?finalcnt:-1;
}

int CSemaphore::tryPend()
{
	int ret=-1;
	ret=pthread_mutex_trylock(&mInternal->mtx);
	bool bzero=false;//可以加解锁，但是资源为0
	if(0 == ret){
		if(mInternal->mCount > 0){
			mInternal->mCount--;
			bzero=false;
		}
		else{
			bzero=true;
		}
		ret = pthread_mutex_unlock(&mInternal->mtx);
	}

	//资源为0，即使解锁成功也返回-1.
	ret=bzero?-1:ret;
	return (ret == 0)?0:-1;
}