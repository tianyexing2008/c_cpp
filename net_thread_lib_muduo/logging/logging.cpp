#include "logging.h"
#include "thread.h"
#include <assert.h>

__thread time_t t_lastSecond;  //线程本地存储,保存最后一次的秒数
__thread char t_time[64];
__thread char t_errnoBuf[128];

const char *strerror_tl(int saveError)
{
    return strerror_r(saveError, t_errnoBuf, sizeof(t_errnoBuf));
}
CLogger::LogLevel initLogLevel()
{
    if(::getenv("MY_LOG_TRACE"))
    {
        return CLogger::TRACE;
    }
    else if(::getenv("MY_LOG_DEBUG"))
    {
        return CLogger::DEBUG;
    }
    else
    {
        return CLogger::INFO;
    }
}

void defaultOutPut(const char *msg, int len)
{
    size_t n = fwrite(msg, 1, len, stdout);
    (void)n;
}

void defaultFlush()
{
    fflush(stdout);
}

CLogger::LogLevel g_logLevel = initLogLevel();
CLogger::outPutFunc g_output = defaultOutPut;
CLogger::flushFunc g_flush = defaultFlush;

const char *logLevelName[CLogger::MAX_LEVEL] = {
    "TRACE ",
    "DEBUG ",
    "INFO  ",
    "WARN  ",
    "ERROR ",
    "FATAL ",
};

class T
{
public:
    T(const char *str, unsigned len): mData(str), mLen(len)
    {
        assert(strlen(str) == len);
    }
    const char *mData;
    const unsigned mLen;
};

inline CLogStream &operator<<(CLogStream& s, T v)
{
    s.append(v.mData, v.mLen);
    return s;
}

inline CLogStream& operator<<(CLogStream& s, const CLogger::CSourceFile& v)
{
    s.append(v.mData, v.mSize);
    return s;
}

CLogger::LogLevel CLogger::logLevel()
{
    return g_logLevel;
}
void CLogger::setLogLevel(LogLevel level)
{
    g_logLevel = level;
}
void CLogger::setOutPut(outPutFunc output)
{
    g_output = output;

}
void CLogger::setFlush(flushFunc flush)
{
    g_flush = flush;
}

CLogger::CImpl::CImpl(LogLevel level, int old_error, const CSourceFile file, int line, std::string func)
:mTime(CTimestamp::now())
,mStream()
,mLevel(level)
,mLine(line)
,mBaseName(file)
,mFunction(func)
{
    formatTime();
    muduo::CurrentThread::tid();
    mStream << "tid:" << T(muduo::CurrentThread::tidString(), muduo::CurrentThread::tidStringLength());
    mStream << T(logLevelName[level], 6);
    if(old_error != 0)
    {
        mStream << strerror_tl(old_error) << "(errno=" << old_error << ")";
    }
    finish();
}

void CLogger::CImpl::formatTime()
{
    int64_t microSecondsSinceEpoch = mTime.microSecondsSinceEpoch();
    time_t seconds = static_cast<time_t>(microSecondsSinceEpoch / CTimestamp::mMicroSecondPerSecond);
    int microSeconds = static_cast<int>(microSecondsSinceEpoch % CTimestamp::mMicroSecondPerSecond);
    if(seconds != t_lastSecond)
    {
        t_lastSecond = seconds;
        struct tm tmTime;
        localtime_r(&seconds, &tmTime); //使用本地时间

        int len = snprintf(t_time, sizeof(t_time), "[%04d-%02d-%02d %02d:%02d:%02d",
                tmTime.tm_year + 1900, tmTime.tm_mon + 1, tmTime.tm_mday,
                tmTime.tm_hour, tmTime.tm_min, tmTime.tm_sec);
        assert(len == 20);
        (void)len;
    }

    CFmt us(".%06d] ", microSeconds);
    mStream << T(t_time, 20) << T(us.data(), 9);
}

void CLogger::CImpl::finish()
{
    mStream << mFunction << "-" << "[" << mBaseName << ":" << mLine << "] ";
}

CLogger::CLogger(CSourceFile file, int line)
:mImpl(INFO, 0, file, line)
{

}
CLogger::CLogger(CSourceFile file, int line, LogLevel level)
:mImpl(level, 0, file, line)
{

}
CLogger::CLogger(CSourceFile file, int line, LogLevel level, const char *func)
:mImpl(level, 0, file, line, func)
{
    //mImpl.mStream << func << ' ';
}
CLogger::CLogger(CSourceFile file, int line, bool toAbort)
:mImpl(toAbort?FATAL:ERROR, errno, file, line)
{

}
CLogger::~CLogger()
{
    //mImpl.finish();
    stream() << "\n";
    const CLogStream::Buffer &buf(stream().buffer());
    g_output(buf.data(), buf.lenght());
    if(mImpl.mLevel == FATAL)
    {
        g_flush();
        abort();
    }
}