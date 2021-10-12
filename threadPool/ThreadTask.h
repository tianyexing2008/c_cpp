#ifndef __THREAD_TASK_H__
#define __THREAD_TASK_H__

#include "Function.h"

class IThreadTask
{
public:
    virtual ~IThreadTask(){};

    /**
     * @brief 触发回调
     * 
     */
    virtual void trigger() = 0;
};

#endif