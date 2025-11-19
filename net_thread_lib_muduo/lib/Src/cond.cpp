#include "condition.h"
#include "guard.h"
#include <errno.h>
#include <pthread.h>
#include <inttypes.h>

Condition::Condition(CMutex &mutex): mMtx(mutex)
{
    pthread_cond_init(&mCond, nullptr);
}


Condition::~Condition()
{
    pthread_cond_destroy(&mCond);
}

void Condition::wait()
{
    pthread_cond_wait(&mCond, mMtx.getMutex());
}

void Condition::notify()
{
    pthread_cond_signal(&mCond);
}

void Condition::notifyAll()
{
    pthread_cond_broadcast(&mCond);
}

bool Condition::waitForSeconds(double seconds)
{
    struct timespec abstime;
    // FIXME: use CLOCK_MONOTONIC or CLOCK_MONOTONIC_RAW to prevent time rewind.
    clock_gettime(CLOCK_REALTIME, &abstime);

    const int64_t kNanoSecondsPerSecond = 1000000000;
    int64_t nanoseconds = static_cast<int64_t>(seconds * kNanoSecondsPerSecond);

    abstime.tv_sec += static_cast<time_t>((abstime.tv_nsec + nanoseconds) / kNanoSecondsPerSecond);
    abstime.tv_nsec = static_cast<long>((abstime.tv_nsec + nanoseconds) % kNanoSecondsPerSecond);

    CGuard ug(mMtx);
    return ETIMEDOUT == pthread_cond_timedwait(&mCond, mMtx.getMutex(), &abstime);
}