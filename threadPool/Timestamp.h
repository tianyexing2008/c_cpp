#ifndef __TIMESTAMP_H__
#define __TIMESTAMP_H__

#include <stdint.h>
#include <string>

class Timestamp
{
public:
    ///
    /// Constucts an invalid Timestamp.
    ///
    Timestamp();

    ///
    /// Constucts a Timestamp at specific time
    ///
    /// @param microSecondsSinceEpoch
    explicit Timestamp(int64_t microSecondsSinceEpoch);

    void swap(Timestamp& that)
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
    static Timestamp now();

    ///
    /// Get an invalid time.
    ///
    static Timestamp invalid();

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t mMicroSecondsSinceEpoch;
};

inline bool operator<(Timestamp lhs, Timestamp rhs)
{
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs)
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
inline double timeDifference(Timestamp high, Timestamp low)
{
    int64_t diff = high.microSecondsSinceEpoch() - low.microSecondsSinceEpoch();
    return static_cast<double>(diff) / Timestamp::kMicroSecondsPerSecond;
}

///
/// Add @c seconds to given timestamp.
///
/// @return timestamp+seconds as Timestamp
///
inline Timestamp addTime(Timestamp timestamp, double seconds)
{
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

#endif
