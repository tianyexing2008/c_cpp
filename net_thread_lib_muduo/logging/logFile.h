#ifndef __LOG_FILE_H__
#define __LOG_FILE_H__

#include "noncopyable.h"
#include "mutex.h"
#include <memory>
#include <string>

namespace FileUtil
{
    class CAppendFile;
}

class CLogFile: muduo::noncopyable
{
public:
    CLogFile(const std::string &basename, off_t rollSize, bool threadSafe = true, int flushInterval = 3, int checkEveryN = 1024);
    ~CLogFile();
    void append(const char *msg, int len);
    void flush();
    bool rollFile();

private:
    static std::string getLogFileName(const std::string &basename, time_t *now);
    void appendUnlock(const char *msg, int len);
    const std::string mBaseName;
    const off_t mRollSize;
    const int mFlushInterval;
    const int mCheckEveryN;
    int mCount;
    std::unique_ptr<CMutex> mMutex;
    time_t mStartOfPeriod;
    time_t mLastRoll;
    time_t mLastFlush;
    std::unique_ptr<FileUtil::CAppendFile> mFile;
    const static int kRollPerSeconds = 60*60*24;
};
#endif