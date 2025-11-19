#include "guard.h"
#include "countdownlatch.h"

namespace muduo
{


CountDownLatch::CountDownLatch(int count): mCount(count), mCond(mMutex)
{

}

void CountDownLatch::wait()
{
    CGuard guard(mMutex); //cause deadlock
    while(mCount > 0) 
    {
        mCond.wait();
    }
}

void CountDownLatch::countDown()
{
    CGuard guard(mMutex);
    --mCount;
    if(0 == mCount)
    {
        mCond.notifyAll();
    }
}

int CountDownLatch::getCount() const
{
    return mCount;
}

}