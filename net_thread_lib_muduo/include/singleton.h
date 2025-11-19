
#ifndef __SINGLETON_H__
#define __SINGLETON_H__
#include <pthread.h>
#include "noncopyable.h"

template<typename T>
class CSingleton: public muduo::noncopyable
{
public:
    static T& getInstance()
    {
        pthread_once(&pOnce, &CSingleton::init);
        return *instance;
    }

    static void init()
    {
        instance = new T();
        static Destructor destructor; //利用static 变量在程序退出时析构，释放 instance
        printf("in multiThread init once\n");
    }

    class Destructor
    {
    public:
         ~Destructor()
         {
            if(CSingleton::instance != NULL)
            {
                delete CSingleton::instance;
                CSingleton::instance = NULL;
            }
         }
    };
private:
    CSingleton();
    ~CSingleton();
private:
    static pthread_once_t pOnce;
    static T *instance;
};


template<typename T>
pthread_once_t CSingleton<T>::pOnce = PTHREAD_ONCE_INIT;

template<typename T>
T *CSingleton<T>::instance = NULL;

#endif