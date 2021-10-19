#include "LogFile.h"
// #include <muduo/base/Logging.h> // strerror_tl
// #include <muduo/base/ProcessInfo.h>

#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <string.h>
#include "Guard.h"
#include "ProcessInfo.h"

// not thread safe
class LogFile::CFile : boost::noncopyable
{
public:
    explicit CFile(const std::string& filename): mFp(::fopen(filename.c_str(), "ae")), mWrittenBytes(0)
    {
        assert(mFp);
        ::setbuffer(mFp, mBuffer, sizeof mBuffer);
        // posix_fadvise POSIX_FADV_DONTNEED ?
    }

    ~CFile()
    {
        ::fclose(mFp);
    }

    void append(const char* logline, const size_t len)
    {
        size_t n = write(logline, len);
        size_t remain = len - n;
        while (remain > 0)
        {
            size_t x = write(logline + n, remain);
            if (x == 0)
            {
                int err = ferror(mFp);
                if (err)
                {
                    fprintf(stderr, "LogFile::File::append() failed %s\n", strerror(err));
                }
                break;
            }
            n += x;
            remain = len - n; // remain -= x
        }

        mWrittenBytes += len;
    }

    void flush()
    {
        ::fflush(mFp);
    }

    size_t writtenBytes() const 
    { 
        return mWrittenBytes; 
    }

private:
    size_t write(const char* logline, size_t len)
    {
#undef fwrite_unlocked
        return ::fwrite_unlocked(logline, 1, len, mFp);
    }

    FILE* mFp;
    char mBuffer[64*1024];
    size_t mWrittenBytes;
};

LogFile::LogFile(const std::string& basename, size_t rollSize, bool threadSafe, int flushInterval):
mBasename(basename),
mRollSize(rollSize),
mFlushInterval(flushInterval),
mCount(0),
mMutex(threadSafe ? new CMutex : NULL),
mStartOfPeriod(0),
mLastRoll(0),
mLastFlush(0)
{
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

LogFile::~LogFile()
{

}

void LogFile::append(const char* logline, int len)
{
    if (mMutex)
    {
        CGuard lock(*mMutex);
        append_unlocked(logline, len);
    }
    else
    {
        append_unlocked(logline, len);
    }
}

void LogFile::flush()
{
    if(mMutex)
    {
        CGuard lock(*mMutex);
        mFile->flush();
    }
    else
    {
        mFile->flush();
    }
}

void LogFile::append_unlocked(const char* logline, int len)
{
    mFile->append(logline, len);

    if (mFile->writtenBytes() > mRollSize)
    {
        rollFile();
    }
    else
    {
        if (mCount > kCheckTimeRoll_)
        {
            mCount = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod_ = now / kRollPerSeconds_ * kRollPerSeconds_;
            if (thisPeriod_ != mStartOfPeriod)
            {
                rollFile();
            }
            else if (now - mLastFlush > mFlushInterval)
            {
                mLastFlush = now;
                mFile->flush();
            }
        }
        else
        {
            ++mCount;
        }
    }
}

void LogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(mBasename, &now);
    time_t start = now / kRollPerSeconds_ * kRollPerSeconds_;

    if (now > mLastRoll)
    {
        mLastRoll = now;
        mLastFlush = now;
        mStartOfPeriod = start;
        mFile.reset(new CFile(filename));
    }
}

std::string LogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    char pidbuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm); // FIXME: localtime_r ?
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;
    filename += ProcessInfo::hostname();
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;
    filename += ".log";

    return filename;
}

