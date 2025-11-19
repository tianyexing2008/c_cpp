#include <assert.h>
#include "guard.h"
#include "logFile.h"
#include "timestamp.h"
#include "asyncLogging.h"

CAsyncLogging::CAsyncLogging(const std::string &basename, off_t rollSize, int flushInterval)
:mFlushInterval(flushInterval)
,mRunning(false)
,mBasename(basename)
,mRollSize(rollSize)
,mThread(std::bind(&CAsyncLogging::threadFunc, this), "asyncLogging")
,mLatch(1)
,mCond(mMutex)
,mCurrentBuffer(new Buffer)
,mNextBuffer(new Buffer)
,mBuffers()
{
    mCurrentBuffer->bzero();
    mNextBuffer->bzero();
    mBuffers.reserve(16);
}

void CAsyncLogging::append(const char *msg, int len)
{
    CGuard lock(mMutex);
    if(mCurrentBuffer->avail() > len) //还能写入的就直接写
    {
        mCurrentBuffer->append(msg, len);
    }
    else
    {
        mBuffers.push_back(std::move(mCurrentBuffer)); //所有权被移到容器内部，mCurrentBuffer被置为空,即不再拥有原指针的所有权
        if(mNextBuffer)
        {
            mCurrentBuffer = std::move(mNextBuffer);
        }
        else
        {
            mCurrentBuffer.reset(new Buffer);
        }
        mCurrentBuffer->append(msg, len);
        mCond.notify();
    }
}

void CAsyncLogging::threadFunc()
{
    
    assert(mRunning == true);
    //mLatch.countDown();
    
    CLogFile output(mBasename, mRollSize, false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite; //用于交换，在锁内交换，在锁外安全操作，避免锁区域过大。
    buffersToWrite.reserve(16);
    
    while(mRunning)
    {
        assert(newBuffer1 && newBuffer1->lenght() == 0);
        assert(newBuffer2 && newBuffer2->lenght() == 0);
        assert(buffersToWrite.empty());
        
        {
            CGuard lock(mMutex);
            if(mBuffers.empty())
            {
                mCond.waitForSeconds(mFlushInterval);
            }
            mBuffers.push_back(std::move(mCurrentBuffer));
            mCurrentBuffer = std::move(newBuffer1);
            buffersToWrite.swap(mBuffers);
            if(!mNextBuffer)
            {
                mNextBuffer = std::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());
        if(buffersToWrite.size() > 25)
        {
            char buf[256] = {0};
            snprintf(buf, sizeof(buf), "Drop log message at %s, %zd larger buffers\n",
                    CTimestamp::now().toFormatedString().c_str(), buffersToWrite.size() - 2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin() + 2, buffersToWrite.end());
        }

        for(const auto &buffer : buffersToWrite)
        {
            printf("prepare write to file!\n");
            output.append(buffer->data(), buffer->lenght());
        }

        //保留2个缓冲区newBuffer1 和 newBuffer2 复用，避免重复 new 
        if(buffersToWrite.size() > 2)
        {
            buffersToWrite.resize(2);
        }

        //给 newBuffer1 复用，并调用 reset（）清空内容
        if(!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer1->reset();          
        }

        //给 newBuffer2 复用，并调用 reset（）清空内容
        if(!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = std::move(buffersToWrite.back());
            buffersToWrite.pop_back();
            newBuffer2->reset();            
        }

        buffersToWrite.clear();
        output.flush();
    }

    output.flush();
}