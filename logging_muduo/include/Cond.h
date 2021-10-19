#ifndef __CONDITION_H__
#define __CONDITION_H__

struct CondInternal;
class CCondition
{
    ///禁用拷贝构造和赋值运算
    CCondition(const CCondition&);
    CCondition& operator=(const CCondition&);
public:
    CCondition();
    ~CCondition();

    void wait();

    void waitForSeconds(int seconds);

    void notifyOne();

    void notifyAll();

private:
    CondInternal *mInternal;
};

#endif