#include "AsyncLogging.h"
// #include <muduo/base/Logging.h>
#include "Timestamp.h"

#include <stdio.h>
#include <sys/resource.h>
#include "LogCore.h"

int kRollSize = 500*1000*1000;

CAsyncLogging* g_asyncLog = NULL;

///重定向到文件
void asyncOutput(const char* msg, int len)
{
    g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
    // CLogger::setOutput(asyncOutput);
    CLogger::setLogLevel(CLogger::TRACE);

    int cnt = 0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string longStr(3000, 'X');
    longStr += " ";

    for (int t = 0; t < 30; ++t)
    {
       CTimestamp start = CTimestamp::now();
        for (int i = 0; i < kBatch; ++i)
        {
            LOG_INFO << "Hello 0123456789" << " abcdefghijklmnopqrstuvwxyz "
                    << (longLog ? longStr : empty)
                    << cnt;
            ++cnt;
        }
        CTimestamp end = CTimestamp::now();
        printf("%f\n", timeDifference(end, start)*1000000/kBatch);
        struct timespec ts = { 0, 500*1000*1000 };
        nanosleep(&ts, NULL);
    }
}

int main(int argc, char* argv[])
{
    {
        // set max virtual memory to 2GB.
        size_t kOneGB = 1000*1024*1024;
        rlimit rl = { 2*kOneGB, 2*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    
    char name[256];
    strncpy(name, argv[0], 256);
    CAsyncLogging log(::basename(name), kRollSize);
    log.start();
    g_asyncLog = &log;

    bool longLog = argc > 1;
    // bench(longLog);
    LOG_INFO << "loggingTest!";

    return 0;
}
