#include "timestamp.h"
#include <time.h>
#include <sys/time.h>

static_assert(sizeof(CTimestamp) == sizeof(int64_t), "CTimestamp should be same size as int64_T");

std::string CTimestamp::toString()const
{
    char buf[32] = {0};
    int64_t seconds = mMicroSecondSinceEpoch / mMicroSecondPerSecond;
    int64_t microSec = mMicroSecondSinceEpoch % mMicroSecondPerSecond;
    snprintf(buf, sizeof(buf), "%" PRId64 ".%06" PRId64, seconds, microSec);
    return buf;
}

std::string CTimestamp::toFormatedString(bool showMicroSec)const
{
    char buf[64] = {0};
    time_t seconds = static_cast<time_t>(mMicroSecondSinceEpoch / mMicroSecondPerSecond);
    
    //struct tm {
    //int tm_sec;    /* Seconds (0-60) */
    //int tm_min;    /* Minutes (0-59) */
    //int tm_hour;   /* Hours (0-23) */
    //int tm_mday;   /* Day of the month (1-31) */
    //int tm_mon;    /* Month (0-11) */
    //int tm_year;   /* Year - 1900 */
    //int tm_wday;   /* Day of the week (0-6, Sunday = 0) */
    //int tm_yday;   /* Day in the year (0-365, 1 Jan = 0) */
    //int tm_isdst;  /* Daylight saving time */
    //};
    struct tm tm_time;
     
    localtime_r(&seconds, &tm_time);
    if(showMicroSec)
    {
        int microSecs = mMicroSecondSinceEpoch % mMicroSecondPerSecond;
        snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%06d", 
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec, microSecs);
    }
    else
    {
        snprintf(buf, sizeof(buf), "%4d%02d%02d %02d:%02d:%02d", 
                tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
                tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec);        
    }

    return buf;
}

CTimestamp CTimestamp::now()
{
/*
    struct timeval {
    time_t      tv_sec;   // 秒数（自1970-01-01 00:00:00 UTC起）
    suseconds_t tv_usec;  // 微秒数（0-999999）
};*/
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return CTimestamp(seconds * CTimestamp::mMicroSecondPerSecond + tv.tv_usec);
}