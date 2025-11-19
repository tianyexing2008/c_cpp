#include "counter.h"
#include "guard.h"
int64_t Counter::value()const
{
    CGuard lock(mutex_);
    return value_;
}

int64_t Counter::getAndIncrease()
{
    CGuard lock(mutex_);
    int64_t ret = value_++; 
    return ret;
}

Counter& Counter::operator=(const Counter &rhs)
{
    if(this == &rhs)
    {
        return *this;
    }
    CGuard lock(mutex_);
    CGuard itsLocak(rhs.mutex_);
    value_ = rhs.value_;
    return *this;
}
