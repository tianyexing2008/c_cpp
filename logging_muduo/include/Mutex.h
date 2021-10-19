#ifndef __MUTEX_H__
#define __MUTEX_H__

struct MutexInternal;
class CMutex
{
    ///禁用赋值和拷贝构造
    CMutex(const CMutex&);
    CMutex &operator=(const CMutex&);
public:
    CMutex();

    ~CMutex();

    /**
     * @brief 进入临界区
     * @return true 
     * @return false 
     */
    bool enter();

    /**
     * @brief 尝试进入临界区
     * @return true 
     * @return false 
     */
    bool tryEnter();

    /**
     * @brief 离开临界区
     * 
     * @return true 
     * @return false 
     */
    bool leave();


private:
    MutexInternal   *mInternal;
};


#endif