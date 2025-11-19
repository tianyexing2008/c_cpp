#include "mutex.h"
#include "Types.h"

class Counter
{
public:
    Counter():value_(0)
    {}

    int64_t value()const;
    int64_t getAndIncrease();
    Counter &operator=(const Counter &rhs);
    int64_t  value_;
    mutable CMutex mutex_;
};
