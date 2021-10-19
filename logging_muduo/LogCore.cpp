#include "LogCore.h"
#include "CurrentThread.h"
#include "StringPiece.h"
#include "Timestamp.h"

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <assert.h>


/*
class LoggerImpl
{
 public:
  typedef CLogger::LogLevel LogLevel;
  LoggerImpl(LogLevel level, int old_errno, const char* file, int line);
  void finish();

  Timestamp mTime;
  CLogStream mStream;
  LogLevel mLevel;
  int mLine;
  const char* fullname_;
  const char* mBasename;
};
*/

__thread char t_errnobuf[512];
__thread char t_time[32];
__thread time_t t_lastSecond;

const char* strerror_tl(int savedErrno)
{
    return strerror_r(savedErrno, t_errnobuf, sizeof t_errnobuf);
}

CLogger::LogLevel initLogLevel()
{
    if (::getenv("MUDUO_LOG_TRACE"))
    {
        return CLogger::TRACE;
    }
    else if (::getenv("MUDUO_LOG_DEBUG"))
    {
        return CLogger::DEBUG;
    }
    else
    {
        return CLogger::INFO;
    }
}

CLogger::LogLevel g_logLevel = initLogLevel();

const char* LogLevelName[CLogger::NUM_LOG_LEVELS] =
{
  "TRACE ",
  "DEBUG ",
  "INFO  ",
  "WARN  ",
  "ERROR ",
  "FATAL ",
};

// helper class for known string length at compile time
class T
{
public:
    T(const char* str, unsigned len): mStr(str), mLen(len)
    {
        assert(strlen(str) == mLen);
    }

    const char* mStr;
    const unsigned mLen;
};

inline CLogStream& operator<<(CLogStream& s, T v)
{
    s.append(v.mStr, v.mLen);
    return s;
}

inline CLogStream& operator<<(CLogStream& s, const CLogger::SourceFile& v)
{
    s.append(v.mData, v.mSize);
    return s;
}

void defaultOutput(const char* msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    //FIXME check n
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

CLogger::OutputFunc g_output = defaultOutput;
CLogger::FlushFunc g_flush = defaultFlush;


CLogger::Impl::Impl(LogLevel level, int savedErrno, const SourceFile& file, int line):
mTime(CTimestamp::now()),
mStream(),
mLevel(level),
mLine(line),
mBasename(file)
{
    formatTime();
    CurrentThread::tid();
    const char *threadStr = CurrentThread::tidString();
    mStream << "tid: " << T(threadStr, strlen(threadStr));
    mStream << T(LogLevelName[level], 6);
    if (savedErrno != 0)
    {
        mStream << strerror_tl(savedErrno) << " (errno=" << savedErrno << ") ";
    }
}

void CLogger::Impl::formatTime()
{
    int64_t microSecondsSinceEpoch = mTime.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / 1000000);
    int microseconds = static_cast<int>(microSecondsSinceEpoch % 1000000);

    //秒数不一样时才进行时间转换，提高效率
    if (seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tm_time;
        // ::gmtime_r(&seconds, &tm_time); // FIXME TimeZone::fromUtcTime
        
        //使用本地时间
        ::localtime_r(&seconds, &tm_time);
        int len = snprintf(t_time, sizeof(t_time), "%4d%02d%02d %02d:%02d:%02d",
            tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
            tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);
        assert(len == 17); (void)len;
    }
    Fmt us(".%06d ", microseconds);
    mStream << T(t_time, 17) << T(us.data(), 8);
}

void CLogger::Impl::finish()
{
    ///日志输出最后输出文件名、行号
    mStream << " - " << mBasename << ':' << mLine << '\n';
}

CLogger::CLogger(SourceFile file, int line): mImpl(INFO, 0, file, line)
{
}

CLogger::CLogger(SourceFile file, int line, LogLevel level, const char* func)
  : mImpl(level, 0, file, line)
{
    mImpl.mStream << func << ' ';
}

CLogger::CLogger(SourceFile file, int line, LogLevel level): mImpl(level, 0, file, line)
{
}

CLogger::CLogger(SourceFile file, int line, bool toAbort): mImpl(toAbort?FATAL:ERROR, errno, file, line)
{
}

CLogger::~CLogger()
{
    mImpl.finish();
    const CLogStream::Buffer& buf(stream().buffer());
    g_output(buf.data(), buf.length());

    if (mImpl.mLevel == FATAL)
    {
        g_flush();
        abort();
    }
}

void CLogger::setLogLevel(CLogger::LogLevel level)
{
    g_logLevel = level;
}

void CLogger::setOutput(OutputFunc out)
{
    g_output = out;
}

void CLogger::setFlush(FlushFunc flush)
{
    g_flush = flush;
}
