#include "mutex.h"
#include <assert.h>

struct mutexInternal
{
    pthread_mutex_t mtx;
};

CMutex::CMutex()
{
    mInternal = new mutexInternal;
    int rt = pthread_mutex_init(&mInternal->mtx, nullptr);
    assert(rt == 0);
}

CMutex::~CMutex()
{
    int rt = pthread_mutex_destroy(&mInternal->mtx);
    assert(rt == 0);
    delete mInternal;
    mInternal = nullptr;
}

bool CMutex::enter()
{
    return (pthread_mutex_lock(&mInternal->mtx) == 0);
}

bool CMutex::leave()
{
    return (pthread_mutex_unlock(&mInternal->mtx) == 0);
}

bool CMutex::tryEnter()
{
    return (pthread_mutex_trylock(&mInternal->mtx) == 0);
}

pthread_mutex_t *CMutex::getMutex()
{
    return &mInternal->mtx;
}