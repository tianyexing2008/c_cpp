
#ifndef MUDUO_BASE_COUNTDOWNLATCH_H
#define MUDUO_BASE_COUNTDOWNLATCH_H

#include "condition.h"
#include "mutex.h"
#include "noncopyable.h"

namespace muduo
{

class CountDownLatch : public noncopyable
{
public:

  explicit CountDownLatch(int count); //倒数次数

  void wait();  //等待计数值变为0

  void countDown(); //计数减1

  int getCount() const;

private:
  mutable CMutex mMutex;
  Condition mCond;
  
  int mCount;
};

}  // namespace muduo
#endif  // MUDUO_BASE_COUNTDOWNLATCH_H
