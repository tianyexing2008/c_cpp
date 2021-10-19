

#include <pthread.h>
#include "Cond.h"

struct CondInternal
{
    pthread_mutex_t mMutex;
    pthread_cond_t mCond;
};

CCondition::CCondition()
{
    mInternal = new CondInternal;
    pthread_mutex_init(&mInternal->mMutex, nullptr);
    pthread_cond_init(&mInternal->mCond, nullptr);
}

CCondition::~CCondition()
{
    pthread_cond_destroy(&mInternal->mCond);
    pthread_mutex_destroy(&mInternal->mMutex);
}

void CCondition::wait()
{
    pthread_cond_wait(&mInternal->mCond, &mInternal->mMutex);
}

// returns true if time out, false otherwise.
void CCondition::waitForSeconds(int seconds)
{
    struct timespec timeout;
    clock_gettime(CLOCK_REALTIME, &timeout);
    timeout.tv_sec += seconds;
    pthread_cond_timedwait(&mInternal->mCond, &mInternal->mMutex, &timeout);
}

void CCondition::notifyOne()
{
    pthread_cond_signal(&mInternal->mCond);
}

void CCondition::notifyAll()
{
    pthread_cond_broadcast(&mInternal->mCond);
}

