#include "Mutex.h"
#include <assert.h>
#include <pthread.h>

struct MutexInternal
{
    pthread_mutex_t mutex;
};

CMutex::CMutex()
{
    mInternal = new MutexInternal;
    int ret = pthread_mutex_init(&mInternal->mutex, nullptr);
    assert(ret == 0);
}

CMutex::~CMutex()
{
    mInternal = new MutexInternal;
    int ret = pthread_mutex_destroy(&mInternal->mutex);
    assert(ret == 0);
    delete mInternal;
    mInternal = nullptr;
}

bool CMutex::enter()
{
    return (pthread_mutex_lock(&mInternal->mutex) == 0);
}

bool CMutex::tryEnter()
{
    return (pthread_mutex_trylock(&mInternal->mutex) == 0);
}

bool CMutex::leave()
{
    return (pthread_mutex_unlock(&mInternal->mutex) == 0);
}
