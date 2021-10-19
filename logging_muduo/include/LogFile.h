#ifndef MUDUO_BASE_LOGFILE_H
#define MUDUO_BASE_LOGFILE_H

#include "Mutex.h"
#include <string>
#include <boost/noncopyable.hpp>
#include <boost/scoped_ptr.hpp>


class LogFile: boost::noncopyable
{
public:
    LogFile(const std::string& basename, size_t rollSize, bool threadSafe = true, int flushInterval = 3);
    ~LogFile();

    void append(const char* logline, int len);

    void flush();

private:
    void append_unlocked(const char* logline, int len);

    static std::string getLogFileName(const std::string& basename, time_t* now);

    void rollFile();

private:
    const std::string mBasename;     //日志文件名
    const size_t mRollSize;     //回滚大小
    const int mFlushInterval;   //刷新间隔

    int mCount;

    boost::scoped_ptr<CMutex> mMutex;
    time_t mStartOfPeriod;
    time_t mLastRoll;
    time_t mLastFlush;
    class CFile;
    boost::scoped_ptr<CFile> mFile;

    const static int kCheckTimeRoll_ = 1024;
    const static int kRollPerSeconds_ = 60*60*24;
};

#endif  // MUDUO_BASE_LOGFILE_H
