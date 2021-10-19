#include "Timestamp.h"
#include <sys/time.h>
#include <stdio.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS


CTimestamp::CTimestamp(): mMicroSecondsSinceEpoch(0)
{
}

CTimestamp::CTimestamp(int64_t microseconds): mMicroSecondsSinceEpoch(microseconds)
{
}

std::string CTimestamp::toString() const
{
    char buf[32] = {0};
    int64_t seconds = mMicroSecondsSinceEpoch / kMicroSecondsPerSecond;
    int64_t microseconds = mMicroSecondsSinceEpoch % kMicroSecondsPerSecond;
    snprintf(buf, sizeof(buf)-1, "%" PRId64 ".%06" PRId64 "", seconds, microseconds);
    return buf;
}

std::string CTimestamp::toFormattedString() const
{
    char buf[32] = {0};
    time_t seconds = static_cast<time_t>(mMicroSecondsSinceEpoch / kMicroSecondsPerSecond);
    int microseconds = static_cast<int>(mMicroSecondsSinceEpoch % kMicroSecondsPerSecond);
    struct tm tm_time;
    localtime_r(&seconds, &tm_time);

    snprintf(buf, sizeof(buf), "%4d-%02d-%02d %02d:%02d:%02d.%02d",
        tm_time.tm_year + 1900, tm_time.tm_mon + 1, tm_time.tm_mday,
        tm_time.tm_hour, tm_time.tm_min, tm_time.tm_sec,
        microseconds);
    return buf;
}

CTimestamp CTimestamp::now()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int64_t seconds = tv.tv_sec;
    return CTimestamp(seconds * kMicroSecondsPerSecond + tv.tv_usec);
}

CTimestamp CTimestamp::invalid()
{
    return CTimestamp();
}
