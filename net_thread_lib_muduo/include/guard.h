#ifndef __GUARD_H__
#define __GUARD_H__
#include "mutex.h"

class CGuard
{
    CGuard(const CGuard&) = delete;
    CGuard& operator=(const CGuard&) = delete;
public:
    CGuard(CMutex &mutex): mMutex(mutex)
    {
        mMutex.enter();
    }

    ~CGuard()
    {
        mMutex.leave();
    }
private:
    CMutex  &mMutex;
};
#endif