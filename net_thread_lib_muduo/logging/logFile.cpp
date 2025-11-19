#include "logFile.h"
#include "guard.h"
#include "processInfo.h"
#include "fileUtil.h"
#include <assert.h>

CLogFile::CLogFile(const std::string &basename, off_t rollSize, bool threadSafe, int flushInterval, int checkEveryN)
:mBaseName(basename)
,mRollSize(rollSize)
,mFlushInterval(flushInterval)
,mCheckEveryN(checkEveryN)
,mCount(0)
,mMutex(threadSafe ? new CMutex : nullptr)
,mStartOfPeriod(0)
,mLastRoll(0)
,mLastFlush(0)
{
    assert(basename.find('/') == std::string::npos);
    rollFile();
}

CLogFile::~CLogFile() = default;

void CLogFile::append(const char *msg, int len)
{
    if(mMutex)
    {
        CGuard lock(*mMutex);
        appendUnlock(msg, len);
    }
    else
    {
        appendUnlock(msg, len);
    }
}

void CLogFile::flush()
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

void CLogFile::appendUnlock(const char *msg, int len)
{
    mFile->append(msg, len);
    if(mFile->writtenBytes() > mRollSize)
    {
        rollFile();
    }
    else
    {
        ++mCount;
        if(mCount > mCheckEveryN)
        {
            mCount = 0;
            time_t now = ::time(NULL);
            time_t thisPeriod = now / kRollPerSeconds * kRollPerSeconds;
            if(thisPeriod != mStartOfPeriod)
            {
                rollFile();
            }
            else if(now - mLastFlush > mFlushInterval)
            {
                mLastFlush = now;
                mFile->flush();
            }
        }
    }
}

bool CLogFile::rollFile()
{
    time_t now = 0;
    std::string filename = getLogFileName(mBaseName, &now);
    time_t start = now / kRollPerSeconds * kRollPerSeconds;

    if(now > mLastRoll)
    {
        mLastRoll = now;
        mLastFlush = now;
        mStartOfPeriod = start;
        mFile.reset(new FileUtil::CAppendFile(filename));
        return true;
    }
    return false;
}

std::string CLogFile::CLogFile::getLogFileName(const std::string& basename, time_t* now)
{
    std::string filename;
    filename.reserve(basename.size() + 64);
    filename = basename;

    char timebuf[32];
    struct tm tm;
    *now = time(NULL);
    gmtime_r(now, &tm); // FIXME: localtime_r ?
    strftime(timebuf, sizeof timebuf, ".%Y%m%d-%H%M%S.", &tm);
    filename += timebuf;

    filename += ProcessInfo::hostname();

    char pidbuf[32];
    snprintf(pidbuf, sizeof pidbuf, ".%d", ProcessInfo::pid());
    filename += pidbuf;

    filename += ".log";

    return filename;
}