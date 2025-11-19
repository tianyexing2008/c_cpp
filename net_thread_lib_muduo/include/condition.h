#ifndef __H_COND__
#define __H_COND__

#include <pthread.h>
#include "mutex.h"

class Condition
{
public:
    Condition(CMutex &mMtx);

    ~Condition();

    void wait();

    void notify();
    
    void notifyAll();

    bool waitForSeconds(double seconds);
private:
    CMutex &mMtx;
    pthread_cond_t mCond;
};

#endif