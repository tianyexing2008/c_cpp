#include "eventLoopThread.h"
#include "eventLoop.h"
#include "guard.h"

using namespace muduo;
using namespace net;

CEventLoopThread::CEventLoopThread(const ThreadInitCallback& cb,
                                 const std::string& name)
  : mLoop(NULL),
    mExiting(false),
    mThread(std::bind(&CEventLoopThread::threadFunc, this), name),
    mMutex(),
    mCond(mMutex),
    mCallback(cb)
{
}

CEventLoopThread::~CEventLoopThread()
{
    mExiting = true;
    if (mLoop != NULL) // not 100% race-free, eg. threadFunc could be running mCallback.
    {
        // still a tiny chance to call destructed object, if threadFunc exits just now.
        // but when CEventLoopThread destructs, usually programming is exiting anyway.
        mLoop->quit();
        mThread.join();
    }
}

CEventLoop* CEventLoopThread::startLoop()
{
    mThread.start();

    CEventLoop* loop = NULL;
    {
        CGuard lock(mMutex);
        while (mLoop == NULL)
        {
            mCond.wait();
        }
        loop = mLoop;
    }

    return loop;
}

void CEventLoopThread::threadFunc()
{
    CEventLoop loop;

    if (mCallback)
    {
        mCallback(&loop);
    }

    {
        CGuard lock(mMutex);
        mLoop = &loop;
        mCond.notify();
    }

    loop.loop();
    CGuard lock(mMutex);
    mLoop = NULL;
}

