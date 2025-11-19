#ifndef __H_MUTEX_H__
#define __H_MUTEX_H__

#include <pthread.h>

struct mutexInternal;
class CMutex
{
    CMutex(const CMutex &);
    CMutex& operator=(const CMutex &);

public:
    CMutex();
    ~CMutex();

    /**
     * brief 进入临界区
     * 
    */
    bool enter();

    /**
     * 尝试进入临界区
    */
    bool tryEnter();

    /**
     * 离开临界区
    */
    bool leave();

    pthread_mutex_t *getMutex();
private:
    mutexInternal *mInternal;
};
#endif