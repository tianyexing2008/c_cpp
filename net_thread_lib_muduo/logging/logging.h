#ifndef __LOGGING_H__
#define __LOGGING_H__

#include "logStream.h"
#include "timestamp.h"

class CLogger
{
public:
    enum LogLevel
    {
        TRACE,
        DEBUG,
        INFO,
        WARN,
        ERROR,
        FATAL,
        MAX_LEVEL,
    };

    class CSourceFile
    {
    public:
        template<int N>
        CSourceFile(const char (&arr)[N]):mData(arr), mSize(N - 1)
        {
            const char *slash = strrchr(mData, '/');
            if(slash)
            {
                mData = slash + 1;
                mSize -= static_cast<size_t>(mData - arr);
            }
        }

        explicit CSourceFile(const char *fileName): mData(fileName)
        {
            const char *slash = strrchr(fileName, '/');
            if(slash)
            {
                mData = slash + 1;
            }
            mSize = static_cast<size_t>(strlen(mData));
        }
    
        const char *mData;
        int mSize;
    };

    CLogger(CSourceFile file, int line);
    CLogger(CSourceFile file, int line, LogLevel level);
    CLogger(CSourceFile file, int line, LogLevel level, const char *function);
    CLogger(CSourceFile file, int line, bool toAbort);
    ~CLogger();

    CLogStream& stream() 
    {
        return mImpl.mStream;
    }

    static LogLevel logLevel();
    static void setLogLevel(LogLevel level);

    typedef void (*outPutFunc)(const char *msg, int len);
    typedef void (*flushFunc)();

    static void setOutPut(outPutFunc func);
    static void setFlush(flushFunc func);
private:
    class CImpl
    {
    public:
        typedef CLogger::LogLevel LogLevel;
        CImpl(LogLevel level, int old_error, const CSourceFile file, int line, std::string func="");
        void formatTime();
        void finish();
    
        CTimestamp mTime;
        CLogStream mStream;
        LogLevel mLevel;
        int mLine;
        CSourceFile mBaseName;
        std::string mFunction;
    };

    CImpl mImpl;
};

extern CLogger::LogLevel g_logLevel;

#define LOG_TRACE if(CLogger::logLevel() <= CLogger::TRACE)\
        CLogger(__FILE__, __LINE__, CLogger::TRACE, __FUNCTION__).stream()
#define LOG_INFO if(CLogger::logLevel() <= CLogger::INFO)\
        CLogger(__FILE__, __LINE__, CLogger::INFO, __FUNCTION__).stream()
#define LOG_DEBUG if(CLogger::logLevel() <= CLogger::DEBUG)\
        CLogger(__FILE__, __LINE__, CLogger::DEBUG, __FUNCTION__).stream()

#define LOG_WARN CLogger(__FILE__, __LINE__, CLogger::WARN, __FUNCTION__).stream()
#define LOG_ERROR CLogger(__FILE__, __LINE__, CLogger::ERROR, __FUNCTION__).stream()
#define LOG_FATAL CLogger(__FILE__, __LINE__, CLogger::FATAL, __FUNCTION__).stream()
#define LOG_SYSERR CLogger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL CLogger(__FILE__, __LINE__, true).stream()

template<typename T>
T* checkNotNull(CLogger::CSourceFile file, int len, const char *msg, T* ptr)
{
    if(ptr == nullptr)
    {
        LOG_FATAL << msg << "\n";
    }
    return ptr;
}

#define CHECK_NOT_NULL(val) \
    checkNotNull(__FILE__, __LINE__, "'"#val"' Must be non NULL",(val))
#endif