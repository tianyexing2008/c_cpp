#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <iostream>
#include <signal.h>
#include "Log.h"
#include "ThreadPool.h"
#include "ThreadTask.h"

class CRequestTask: public IThreadTask
{
public:
    typedef TFunction1<void, std::string &> TaskHandler;
    CRequestTask(TaskHandler handler, std::string &msg):
    mHandler(handler),
    mMessage(msg)
    {

    }
    virtual void trigger()
    {
        if(!mHandler.empty())
        {
            mHandler(mMessage);
        }
    }
private:
    TaskHandler mHandler;
    std::string mMessage;
};

void  funcProcTest(std::string &msg)
{
    int times = 10;
    while(times -- > 0)
    {
        tracef("threadPool test, %s\n", msg.c_str());
        sleep(1);
    }
}

///清除颜色打印
void signalFunc(int sig)
{
    printf("\033[0m");
}

int main()
{
    signal(SIGINT, signalFunc);
    auto threadPool = IThreadPool::create();
    threadPool->initialize(4);

    std::string message("liangshen!");
    CRequestTask *testTask = new CRequestTask(funcProcTest, message);
    threadPool->postTask(testTask);

    sleep(20);
    threadPool->deInitialize();
    return 0;
}

