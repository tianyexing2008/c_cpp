#ifndef __ASYNC_LOGGING_H__
#define __ASYNC_LOGGING_H__

#include "Mutex.h"
#include "Cond.h"
#include "Thread.h"
#include "LogStream.h"

#include <string>
#include <boost/bind.hpp>
#include <boost/noncopyable.hpp> //系统已经安装了boost
#include <boost/scoped_ptr.hpp>
#include <boost/ptr_container/ptr_vector.hpp>

class CAsyncLogging: public boost::noncopyable
{

private:
    /// 禁用拷贝构造和赋值
    CAsyncLogging(const CAsyncLogging&);
    CAsyncLogging& operator=(const CAsyncLogging&);

    /**
     * @brief 线程回调函数
     * 
     */
    void threadProc();
public:
    CAsyncLogging(const std::string& basename, size_t rollSize, int flushInterval = 3);
    ~CAsyncLogging();

    void append(const char* message, int len);

    void start();

    void stop();
    
private:
    typedef detail::CFixedBuffer<detail::kLargeBuffer> Buffer;
    typedef boost::ptr_vector<Buffer> BufferVector;
    typedef BufferVector::auto_type BufferPtr;

    const int mFlushInterval;   //刷新缓存到文件间隔，单位秒
    std::string mBasename;
    size_t mRollSize;           //限制文件大小
    CMutex mMutex;
    CCondition  mCond;
    CThread mThread;
    bool mRunning;              //线程是否在运行

    BufferPtr mCurrentBuffer; //当前缓冲
    BufferPtr mNextBuffer;    //预备缓冲
    BufferVector mBuffers;    //待写入文件的已填满的缓冲
};

#endif