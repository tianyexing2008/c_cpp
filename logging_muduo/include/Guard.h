#ifndef __GUARD_H__
#define __GUARD_H__

#include "Mutex.h"

class CGuard 
{
    CGuard(const CGuard&);
    CGuard& operator=(const CGuard&);

public:

    CGuard(CMutex& mutex):mMutex(mutex) {
        mMutex.enter();
    }

    ~CGuard() {
        mMutex.leave();
    }

private:
    CMutex&     mMutex;
};

#endif
