#ifndef UNI_LOGGING_H
#define UNI_LOGGING_H

#include "LogStream.h"
#include "Timestamp.h"

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
        NUM_LOG_LEVELS,
    };

    // compile time calculation of basename of source file
    class SourceFile
    {
    public:
        template<int N>
        inline SourceFile(const char (&arr)[N]): mData(arr), mSize(N-1)
        {
            const char* slash = strrchr(mData, '/'); // builtin function
            if (slash)
            {
                mData = slash + 1;
                mSize -= static_cast<int>(mData - arr);
            }
        }

        explicit SourceFile(const char* filename): mData(filename)
        {
            const char* slash = strrchr(filename, '/');
            if (slash)
            {
                mData = slash + 1;
            }
            mSize = static_cast<int>(strlen(mData));
        }

        const char* mData;
        int mSize;
    };

    CLogger(SourceFile file, int line);
    CLogger(SourceFile file, int line, LogLevel level);
    CLogger(SourceFile file, int line, LogLevel level, const char* func);
    CLogger(SourceFile file, int line, bool toAbort);
    ~CLogger();

    CLogStream& stream() 
    {
        return mImpl.mStream;
    }

    static LogLevel logLevel();

    static void setLogLevel(LogLevel level);

    typedef void (*OutputFunc)(const char* msg, int len);
    typedef void (*FlushFunc)();
    static void setOutput(OutputFunc);
    static void setFlush(FlushFunc);

private:
    class Impl
    {
    public:
        typedef CLogger::LogLevel LogLevel;

        Impl(LogLevel level, int old_errno, const SourceFile& file, int line);

        void formatTime();

        void finish();

        CTimestamp mTime;
        CLogStream mStream;
        LogLevel mLevel;
        int mLine;              //行号
        SourceFile mBasename;   //打印所在的文件名称
    };

    Impl mImpl;
};

extern CLogger::LogLevel g_logLevel;

inline CLogger::LogLevel CLogger::logLevel()
{
  return g_logLevel;
}

// 以下创建的都是匿名对象，在调用完后即销毁，即此语句调用完即调用CLogger的析构函数，
// 在析构函数里进行了IO输出，以此达到即时输出的功能
#define LOG_TRACE if (CLogger::logLevel() <= CLogger::TRACE) \
  CLogger(__FILE__, __LINE__, CLogger::TRACE, __func__).stream()
#define LOG_DEBUG if (CLogger::logLevel() <= CLogger::DEBUG) \
  CLogger(__FILE__, __LINE__, CLogger::DEBUG, __func__).stream()
#define LOG_INFO if (CLogger::logLevel() <= CLogger::INFO) \
  CLogger(__FILE__, __LINE__).stream()
#define LOG_WARN CLogger(__FILE__, __LINE__, CLogger::WARN).stream()
#define LOG_ERROR CLogger(__FILE__, __LINE__, CLogger::ERROR).stream()
#define LOG_FATAL CLogger(__FILE__, __LINE__, CLogger::FATAL).stream()
#define LOG_SYSERR CLogger(__FILE__, __LINE__, false).stream()
#define LOG_SYSFATAL CLogger(__FILE__, __LINE__, true).stream()

const char* strerror_tl(int savedErrno);

// Taken from glog/logging.h
//
// Check that the input is non NULL.  This very useful in constructor
// initializer lists.

#define CHECK_NOTNULL(val) \
  ::CheckNotNull(__FILE__, __LINE__, "'" #val "' Must be non NULL", (val))

// A small helper for CHECK_NOTNULL().
template <typename T>
T* CheckNotNull(CLogger::SourceFile file, int line, const char *names, T* ptr) {
  if (ptr == NULL) {
   CLogger(file, line, CLogger::FATAL).stream() << names;
  }
  return ptr;
}


#endif 
