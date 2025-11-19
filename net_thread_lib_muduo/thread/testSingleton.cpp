#include <stdio.h>
#include <stdlib.h>
#include "thread.h"
#include "singleton.h"

class CSingletonTest
{
public:
    void print()
    {
        printf("singleton test\n");
    }
};

CSingletonTest g_Sington;

void threadFunc(std::string &threadName)
{
    printf("thread %s call getInstance()\n", threadName.c_str());
    g_Sington = CSingleton<CSingletonTest>::getInstance();
}

int main()
{
    for(int i = 0; i < 5; i++)
    {

        muduo::CThread thread(threadFunc, std::to_string(i));
        thread.start();
        thread.join();
    }

    g_Sington.print();

    return 0;
}