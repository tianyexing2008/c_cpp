#ifndef __LOG_H__
#define __LOG_H__
#include <unistd.h>
#include <sys/syscall.h>

static std::string getTimeStamp()
{
    time_t now;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    now = tv.tv_sec;
    struct tm localTime;
    localtime_r(&now, &localTime);
    
    char timeBuf[128] = {0};
    char outBuf[128] = {0};
    strftime((char *)timeBuf, sizeof(timeBuf), "%Y/%m/%d %H:%M:%S", &localTime);
    snprintf(outBuf, sizeof(outBuf), "%s %06u", timeBuf, tv.tv_usec % 1000000);

    return std::string(outBuf);
}

enum COLOR
{
    COLOR_INFO = 32,
    COLOR_WARN = 33, 
    COLOR_ERROR = 31,
    COLOR_FATAL = 36
};

static int s_color[] = {0, COLOR_INFO, COLOR_WARN, COLOR_ERROR, COLOR_FATAL};

#define PRINT(title, level, file, func, line, fmt, ...) \
    printf("\033[%d;40m", s_color[level]); \
    printf("[%s %s]tid:%ld, file: %s, func: %s, line: %d, " fmt"\033[0m", title, getTimeStamp().c_str(), syscall(SYS_gettid), file, func, line, ##__VA_ARGS__)

#define PRINT_INFO(title, level, fmt, ...) \
    PRINT(title, level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

//打印颜色，等级分为1，2，3，4
#define tracef(fmt, ...) PRINT_INFO("trace", 0, fmt, ##__VA_ARGS__)
#define infof(fmt, ...) PRINT_INFO("info", 1, fmt, ##__VA_ARGS__)
#define warnf(fmt, ...) PRINT_INFO("warn", 2, fmt, ##__VA_ARGS__)
#define errorf(fmt, ...) PRINT_INFO("error", 3, fmt, ##__VA_ARGS__)
#define fatalf(fmt, ...) PRINT_INFO("fatal", 4, fmt, ##__VA_ARGS__)

#endif
