#include "asyncLogging.h"
#include "timestamp.h"
#include "logging.h"

#include <stdio.h>
#include <sys/resource.h>
#include <unistd.h>

off_t kRollSize = 500 * 1000 * 1000;
CAsyncLogging *g_asyncLog = NULL;

void asyncOutPut(const char *msg, int len)
{
    g_asyncLog->append(msg, len);
}

void bench(bool longLog)
{
    CLogger::setOutPut(asyncOutPut);
    int cnt =  0;
    const int kBatch = 1000;
    std::string empty = " ";
    std::string longStr(3000, 'X');
    longStr += " ";
    for(int t = 0; t < 3; t++)
    {
        CTimestamp start = CTimestamp::now();
        for(int i = 0; i < kBatch; i++)
        {
            LOG_INFO << "Hello 1234567890" << " abcdefghijklmnopqrstuvwxyz " << cnt << "\n";
            ++cnt;
        }
        CTimestamp end = CTimestamp::now();
        printf("%f\n", timeDifference(end, start) * 1000000 / kBatch);
        struct timespec ts = {1, 0};
        nanosleep(&ts, NULL);
    }
}

int main(int argc, char *argv[])
{
    CLogger::setLogLevel(CLogger::INFO);
    {
        // set max virtual memory to 2GB. 
        size_t kOneGB = 1000*1024*1024;
        rlimit rl = { 2*kOneGB, 2*kOneGB };
        setrlimit(RLIMIT_AS, &rl);
    }

    printf("pid = %d\n", getpid());

    char name[256] = { '\0' };
    strncpy(name, argv[0], sizeof name - 1);
    CAsyncLogging log(::basename(name), kRollSize);
    printf("in main\n");
    log.start();
    g_asyncLog = &log;

    bool longLog = argc > 1;
    bench(longLog);
    log.stop();

    return 0;
}