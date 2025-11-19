#ifndef __ASYNC_LOGGING_H__
#define __ASYNC_LOGGING_H__

#include "noncopyable.h"
#include "fixedBuffer.h"
#include "countdownlatch.h"
#include "thread.h"
#include <atomic>
#include <vector>
#include <memory>

class CAsyncLogging: muduo::noncopyable
{
public:
    CAsyncLogging(const std::string &basename, off_t rollSize, int flushInterval = 3);
    ~CAsyncLogging()
    {
        if(mRunning)
        {
            stop();
        }
    }

    void append(const char *msg, int len);

    void start()
    {
        mRunning = true;
        mThread.start();
        //mLatch.wait();
    }

    void stop()
    {
        mRunning = false;
        //mCond.notify();
        mThread.join();
    }

private:
    void threadFunc();
    typedef CFixedBuffer<gLargeBuffer> Buffer;
    typedef std::vector<std::unique_ptr<Buffer>> BufferVector;
    typedef BufferVector::value_type BufferPtr;

    const int mFlushInterval;
    std::atomic<bool> mRunning;
    const std::string mBasename;
    const off_t mRollSize;
    muduo::CThread mThread;
    muduo::CountDownLatch mLatch;
    CMutex mMutex;
    Condition mCond;
    BufferPtr mCurrentBuffer;
    BufferPtr mNextBuffer;
    BufferVector mBuffers; //就绪的Buffer容器
};

#endif