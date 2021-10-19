#include <mutex>
#include "Guard.h"
#include "Timestamp.h"
#include "AsyncLogging.h"
#include "LogFile.h"

CAsyncLogging::CAsyncLogging(const std::string& basename, size_t rollSize, int flushInterval):
mFlushInterval(flushInterval),
mBasename(basename),
mRollSize(rollSize),
mThread(boost::bind(&CAsyncLogging::threadProc, this), "AsyncLogging"),
mRunning(false),
mCurrentBuffer(new Buffer),
mNextBuffer(new Buffer),
mBuffers()
{
    mCurrentBuffer->bzero();
    mNextBuffer->bzero();
    mBuffers.reserve(16);
}

CAsyncLogging::~CAsyncLogging()
{

}

void CAsyncLogging::start()
{
    mThread.createThread();
    mRunning = true;
}

void CAsyncLogging::stop()
{
    mRunning = false;
    mCond.notifyOne();
    mThread.join();
}

void CAsyncLogging::threadProc()
{
    // assert(mRunning == true);
    // latch_.countDown();
    LogFile output(mBasename, mRollSize, false);
    BufferPtr newBuffer1(new Buffer); //准备两块buffer以备在临界区交换
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector buffersToWrite;
    buffersToWrite.reserve(16);
    
    while(mRunning)
    {
        assert(newBuffer1 && newBuffer1->length() == 0);
        assert(newBuffer2 && newBuffer2->length() == 0);
        assert(buffersToWrite.empty());

        {  //临界区
            CGuard lock(mMutex);
            if (mBuffers.empty())  // unusual usage!
            {
                mCond.waitForSeconds(mFlushInterval); //等待条件成立或超时
            }
            mBuffers.push_back(mCurrentBuffer.release()); //移动，而非复制，将当前缓冲移入mBuffers
            mCurrentBuffer = boost::ptr_container::move(newBuffer1);  //并立刻将空闲的buffer移为当前缓冲
            buffersToWrite.swap(mBuffers);  //内部指针交换，而非复制
            if (!mNextBuffer)
            {
                mNextBuffer = boost::ptr_container::move(newBuffer2);
            }
        }

        assert(!buffersToWrite.empty());

        if (buffersToWrite.size() > 25)
        {
            char buf[256];
            snprintf(buf, sizeof buf, "Dropped log messages at %s, %zd larger buffers\n",
                    CTimestamp::now().toFormattedString().c_str(),
                    buffersToWrite.size()-2);
            fputs(buf, stderr);
            output.append(buf, static_cast<int>(strlen(buf)));
            buffersToWrite.erase(buffersToWrite.begin()+2, buffersToWrite.end());
        }

        for (size_t i = 0; i < buffersToWrite.size(); ++i)
        {
            // FIXME: use unbuffered stdio FILE ? or use ::writev ?
            output.append(buffersToWrite[i].data(), buffersToWrite[i].length());
        }

        if (buffersToWrite.size() > 2)
        {
            // drop non-bzero-ed buffers, avoid trashing
            buffersToWrite.resize(2);
        }

        if (!newBuffer1)
        {
            assert(!buffersToWrite.empty());
            newBuffer1 = buffersToWrite.pop_back();
            newBuffer1->reset();
        }

        if (!newBuffer2)
        {
            assert(!buffersToWrite.empty());
            newBuffer2 = buffersToWrite.pop_back();
            newBuffer2->reset();
        }

        buffersToWrite.clear();
        output.flush();
    }
    output.flush();
}

void CAsyncLogging::append(const char* message, int len)
{
    CGuard lock(mMutex);
    if (mCurrentBuffer->avail() > len)  //剩余空间足够大
    {
        mCurrentBuffer->append(message, len); //则把日志消息拷贝到当前缓冲中
    }
    else    //剩余空间不足以写下len长度的message
    {
        mBuffers.push_back(mCurrentBuffer.release()); //若当前缓冲已满，送入将要写的缓冲中

        if (mNextBuffer)//预备的缓冲
        {
            mCurrentBuffer = boost::ptr_container::move(mNextBuffer); //移用为当前缓冲
        }
        else //若是写得太快导致两块buffer已经写完，则新建一块buffer
        {
            mCurrentBuffer.reset(new Buffer); // Rarely happens
        }
        mCurrentBuffer->append(message, len); //追加日志消息到当前缓冲
        mCond.notifyOne(); //通知后端线程开始写入日志数
    }
}

