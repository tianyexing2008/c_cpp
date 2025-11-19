#ifndef __BLOCKING_QUEUE_H__
#define __BLOCKING_QUEUE_H__
#include "noncopyable.h"
#include "guard.h"
#include "condition.h"
#include <deque>
#include <assert.h>

template<typename T>
class BlockingQueue: public muduo::noncopyable
{
public:
    BlockingQueue(): mNotEmpty(mMutex)
    {

    }

    void put(T &x)
    {
        CGuard lock(mMutex);
        mQueue.push_back(x);
        mNotEmpty.notify();
    }

    T take()
    {
        CGuard lock(mMutex);
        while(mQueue.empty()) //always use while-look, due to spurious wakeup
        {
            mNotEmpty.wait();
        }

        assert(!mQueue.empty());
        T front(mQueue.front());
        mQueue.pop_front();
        return front;
    }

    size_t size()const
    {
        CGuard lock(mMutex);
        return mQueue.size();
    }
private:
    mutable CMutex mMutex;
    Condition mNotEmpty;
    std::deque<T> mQueue;
};
#endif