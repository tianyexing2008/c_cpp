/************************************************
* 				log
* 
* desc: 日志打印相关
* author: kwanson
* email: CSDN kwanson
*************************************************/

#ifndef __PRINTLOG_H__
#define __PRINTLOG_H__

#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>  //for syscall()
#include <sys/syscall.h> // for SYS_gettid

enum CONSOLE_COLOR
{
	COLOR_FATAL = 35,
	COLOR_ERROR = 31,
	COLOR_INFO = 32, 
	COLOR_WARN = 33
};

//等级分为1，2, 3, 4，各等级打印颜色不同
static const int s_color[] = {0, COLOR_INFO, COLOR_WARN, COLOR_ERROR, COLOR_FATAL, COLOR_ERROR};

#define PRINT(title, level, file, func, line, fmt, ...) \
	fprintf(stdout, "\033[%d;40m", s_color[level]); \
	printf("[%s]tid_%ld - %s:%s:%d, " fmt, title, syscall(SYS_gettid), file, func, line, ##__VA_ARGS__); \
	printf("\033[0m");
#define PRINT_INFO(title, level, fmt, ...) \
	PRINT(title, level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define DEBUG
#ifdef DEBUG
#define warnf(fmt, ...) PRINT_INFO("warn", 2, fmt, ##__VA_ARGS__)
#define tracef(fmt, ...) PRINT_INFO("info", 1, fmt, ##__VA_ARGS__)
#define errorf(fmt, ...) PRINT_INFO("error", 3, fmt, ##__VA_ARGS__)
#define errsys(fmt, ...) PRINT_INFO("fatal", 4, fmt, ##__VA_ARGS__)
#else
#define trace(fmt, ...)
#define error(fmt, ...)
#define errsys(fmt, ...)
#endif
#endif