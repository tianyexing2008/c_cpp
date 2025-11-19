
#include "eventLoopThreadPool.h"

#include "eventLoop.h"
#include "eventLoopThread.h"

#include <stdio.h>

using namespace muduo;
using namespace net;

CEventLoopThreadPool::CEventLoopThreadPool(CEventLoop* baseLoop, const std::string& nameArg)
  : mBaseLoop(baseLoop),
    mName(nameArg),
    mStarted(false),
    mNumThreads(0),
    mNext(0)
{
}

CEventLoopThreadPool::~CEventLoopThreadPool()
{
  // Don't delete loop, it's stack variable
}

void CEventLoopThreadPool::start(const ThreadInitCallback& cb)
{
    mBaseLoop->assertInLoopThread();

    mStarted = true;

    for (int i = 0; i < mNumThreads; ++i)
    {
        char buf[mName.size() + 32];
        snprintf(buf, sizeof buf, "%s%d", mName.c_str(), i);
        CEventLoopThread* t = new CEventLoopThread(cb, buf);
        mThreads.push_back(std::unique_ptr<CEventLoopThread>(t));
        mLoops.push_back(t->startLoop());
    }
    if (mNumThreads == 0 && cb)
    {
        cb(mBaseLoop);
    }
}

CEventLoop* CEventLoopThreadPool::getNextLoop()
{
    mBaseLoop->assertInLoopThread();
    CEventLoop* loop = mBaseLoop;

    if (!mLoops.empty())
    {
        // round-robin
        loop = mLoops[mNext];
        ++mNext;
        if (implicit_cast<size_t>(mNext) >= mLoops.size())
        {
            mNext = 0;
        }
    }
    return loop;
}

CEventLoop* CEventLoopThreadPool::getLoopForHash(size_t hashCode)
{
    mBaseLoop->assertInLoopThread();
    CEventLoop* loop = mBaseLoop;

    if (!mLoops.empty())
    {
        loop = mLoops[hashCode % mLoops.size()];
    }
    return loop;
}

std::vector<CEventLoop*> CEventLoopThreadPool::getAllLoops()
{
    mBaseLoop->assertInLoopThread();
    if (mLoops.empty())
    {
        return std::vector<CEventLoop*>(1, mBaseLoop);
    }
    else
    {
        return mLoops;
    }
}
