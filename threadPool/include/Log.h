#ifndef __LOG_H__
#define __LOG_H__
#include <unistd.h>
#include <sys/syscall.h>
#include "../Timestamp.h"

enum COLOR
{
    COLOR_null = 0,
    COLOR_INFO = 32,
    COLOR_WARN = 33, 
    COLOR_ERROR = 31,
    COLOR_FATAL = 36
};

static int s_color[] = {0, COLOR_INFO, COLOR_WARN, COLOR_ERROR, COLOR_FATAL};

///添加{}是消除nowTime重复定义错误
#define PRINT(title, level, file, func, line, fmt, ...) \
    printf("\033[%d;40m", s_color[level]); \
    {   \
    Timestamp nowTime(Timestamp::now());    \
    printf("[%s %s]tid:%ld, file: %s, func: %s, line: %d, " fmt"\033[0m", title, nowTime.toFormattedString().c_str(), syscall(SYS_gettid), file, func, line, ##__VA_ARGS__); \
    }

#define PRINT_INFO(title, level, fmt, ...) \
    PRINT(title, level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

//打印颜色，等级分为1，2，3，4
#define tracef(fmt, ...) PRINT_INFO("trace", 0, fmt, ##__VA_ARGS__)
#define infof(fmt, ...) PRINT_INFO("info", 1, fmt, ##__VA_ARGS__)
#define warnf(fmt, ...) PRINT_INFO("warn", 2, fmt, ##__VA_ARGS__)
#define errorf(fmt, ...) PRINT_INFO("error", 3, fmt, ##__VA_ARGS__)
#define fatalf(fmt, ...) PRINT_INFO("fatal", 4, fmt, ##__VA_ARGS__)

#endif

