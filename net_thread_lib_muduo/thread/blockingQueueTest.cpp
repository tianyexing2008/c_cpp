#include "thread.h"
#include "blockingqueue.h"
#include "countdownlatch.h"
#include <vector>
#include <memory>
#include <string>
#include <stdio.h>
#include <algorithm>
#include <unistd.h>

class Test
{
public:
    Test(int numThreads): mLatch(numThreads)
    {
        mThreads.reserve(numThreads);
        for(int i = 0; i < numThreads; i++)
        {
            char name[32] = {0};
            snprintf(name, sizeof(name), "work_thread_%d", i);
            std::unique_ptr<muduo::CThread> thread(new muduo::CThread(std::bind(&Test::threadProc, this), name));
            mThreads.push_back(std::move(thread));
        }
        for_each(mThreads.begin(), mThreads.end(), std::bind(&muduo::CThread::start, std::placeholders::_1));
    }

    void run(int times)
    {
        printf("waiting for count down latch\n");
        mLatch.wait();
        printf("all threads started\n");
        for(int i = 0; i < times; i++)
        {
            char buf[32] = {0};
            snprintf(buf, sizeof(buf), "hello %d", i);
            std::string buffer(buf);
            mQueue.put(buffer);
            printf("tid = %d, put data = %s, size = %zd\n", muduo::CurrentThread::tid(), buf, mQueue.size());
        }
    }
    void threadProc()
    {
        printf("tid = %d, %s started\n", muduo::CurrentThread::tid(), muduo::CurrentThread::name());
        mLatch.countDown();
        bool running = true;
        while (running)
        {
            std::string d(mQueue.take());
            printf("tid = %d, get data = %s, size = %zd\n", muduo::CurrentThread::tid(), d.c_str(), mQueue.size());
            running = (d != "stop");
        }

        printf("tid=%d, %s stopped\n", muduo::CurrentThread::tid(), muduo::CurrentThread::name());
        
    }

    void joinAll()
    {
        for(size_t i = 0; i < mThreads.size(); i++)
        {
            std::string str("stop");
            mQueue.put(str);

        }
        for_each(mThreads.begin(), mThreads.end(), std::bind(&muduo::CThread::join, std::placeholders::_1));
    }
private:
    BlockingQueue<std::string> mQueue;
    muduo::CountDownLatch mLatch;
    std::vector<std::unique_ptr<muduo::CThread>> mThreads;  
};
int main()
{
    printf("pid=%d, tid=%d\n", ::getpid(), muduo::CurrentThread::tid());
    Test t(5);
    t.run(100);
    t.joinAll();
    printf("number of created threads %d\n", muduo::CThread::numCreate());

    return 0;
}