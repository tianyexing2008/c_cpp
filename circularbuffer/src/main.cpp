#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include "alg.h"
#include "loger/log.h"

#define RUNNING_TIMES 30

void *pushDataThread(void *argc);
void *getDataThread(void *argc);

int main()
{
    pthread_t ptPush, ptGet;


    if(pthread_create(&ptPush, NULL, pushDataThread, NULL) != 0)
    {
        errorf("pthread_create");
        return -1;
    }
    sleep(1);

    if(pthread_create(&ptGet, NULL, getDataThread, NULL) != 0)
    {
        errorf("pthread_create");
        return -1;        
    }

    pthread_join(ptPush, NULL);
    pthread_join(ptGet, NULL);
    return 0;
}


void *pushDataThread(void *argc)
{
    int times = RUNNING_TIMES;
    do
    {
        auto frameInfo = std::make_shared<EncframeinfoAlg>();
        frameInfo->frameinfo = malloc(40);
        if(CAlg::instance()->pushData(frameInfo))
        {
            tracef("pushData success\n");
        }
        sleep(1);
        times -= 1;
    }while(times > 0);

    return NULL;
}

void *getDataThread(void *argc)
{
    std::shared_ptr<EncframeinfoAlg> encFrame = nullptr;
    int times = RUNNING_TIMES;
    do
    {
        if(CAlg::instance()->getData(encFrame))
        {
            //to do
            warnf("getData success\n");
        }
        times -= 1;
        sleep(1); 
    } while (times > 0);
    

    return NULL;
}
