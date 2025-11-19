#ifndef __BASE_TIME_STAMP_H__
#define __BASE_TIME_STAMP_H__

#include "noncopyable.h"
#include <inttypes.h>
#include <string>
#include <utility>

class CTimestamp
{
public:
    CTimestamp():mMicroSecondSinceEpoch(0)
    {

    }
    explicit CTimestamp(int64_t sec):mMicroSecondSinceEpoch(sec)
    {

    }

    CTimestamp(const CTimestamp &other)
    {
        mMicroSecondSinceEpoch = other.mMicroSecondSinceEpoch;
    }

    void swap(CTimestamp & that)
    {
        std::swap(mMicroSecondSinceEpoch, that.mMicroSecondSinceEpoch);
    }
    ~CTimestamp(){}

    bool operator==(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch == other.mMicroSecondSinceEpoch;
    }

    bool operator!=(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch != other.mMicroSecondSinceEpoch; //基于上面的 == 推导
    }

    bool operator<(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch < other.mMicroSecondSinceEpoch;
    }

    bool operator>(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch > other.mMicroSecondSinceEpoch; // 基于 < 推导
    }   

    bool operator<=(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch <= other.mMicroSecondSinceEpoch; // 基于 < 推导
    } 

    bool operator>=(const CTimestamp &other)const
    {
        return mMicroSecondSinceEpoch >= other.mMicroSecondSinceEpoch; // 基于 < 推导
    }

    std::string toString()const;
    std::string toFormatedString(bool showMicroSec = true)const;
    bool valid()const{return mMicroSecondSinceEpoch > 0;}
    int64_t microSecondsSinceEpoch()const{return mMicroSecondSinceEpoch;}
    time_t secondsSinceEpoch()const 
    {
        return static_cast<time_t>(mMicroSecondSinceEpoch / mMicroSecondPerSecond);
    }

    static CTimestamp now();
    static CTimestamp invalid()
    {
        return CTimestamp();
    }

    static CTimestamp formUnixTime(time_t t)
    {
        return formUnixTime(t, 0);
    }

    static CTimestamp formUnixTime(time_t t, int microSeconds)
    {
        return CTimestamp(static_cast<int64_t>(t) * mMicroSecondPerSecond + microSeconds);
    }
    static const int mMicroSecondPerSecond = 1000 * 1000; //每秒等于多少微秒
private:
    
    int64_t mMicroSecondSinceEpoch;
};

/*
inline bool operator<(const CTimestamp &lhs, const CTimestamp &rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(const CTimestamp &lhs, const CTimestamp &rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}
*/
inline double timeDifference(const CTimestamp &high, const CTimestamp &low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / CTimestamp::mMicroSecondPerSecond;
}

inline CTimestamp addTime(CTimestamp &timestamp, double seconds)
{
    int64_t microSecs = static_cast<int64_t>(seconds * CTimestamp::mMicroSecondPerSecond);
    return CTimestamp(timestamp.microSecondsSinceEpoch() + microSecs);
}
#endif