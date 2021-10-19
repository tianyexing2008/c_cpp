#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>
#include <string>

class CTimestamp
{
public:
    ///
    /// Constucts an invalid CTimestamp.
    ///
    CTimestamp();

    ///
    /// Constucts a CTimestamp at specific time
    ///
    /// @param microSecondsSinceEpoch
    explicit CTimestamp(int64_t microSecondsSinceEpoch);

    void swap(CTimestamp& that)
    {
        std::swap(mMicroSecondsSinceEpoch, that.mMicroSecondsSinceEpoch);
    }

    // default copy/assignment/dtor are Okay

    std::string toString() const;
    std::string toFormattedString() const;

    bool valid() const 
    { 
        return mMicroSecondsSinceEpoch > 0; 
    }

    // for internal usage.
    int64_t microSecondsSinceEpoch() const 
    {
        return mMicroSecondsSinceEpoch; 
    }

    ///
    /// Get time of now.
    ///
    static CTimestamp now();

    ///
    /// Get an invalid time.
    ///
    static CTimestamp invalid();

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t mMicroSecondsSinceEpoch;
};

inline bool operator<(CTimestamp lhs, CTimestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(CTimestamp lhs, CTimestamp rhs)
{
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

///
/// Gets time difference of two timestamps, result in seconds.
///
/// @param high, low
/// @return (high-low) in seconds
/// @c double has 52-bit precision, enough for one-microseciond
/// resolution for next 100 years.
inline double timeDifference(CTimestamp high, CTimestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / CTimestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given CTimestamp.
///
/// @return CTimestamp+seconds as CTimestamp
///
inline CTimestamp addTime(CTimestamp Timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * CTimestamp::kMicroSecondsPerSecond);
    return CTimestamp(Timestamp.microSecondsSinceEpoch() + delta);
}

#endif
