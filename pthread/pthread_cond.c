/******************************************************************
* breif: 2个线程对count每次分别加1, 第三个线程等count大于10后一次加100
*******************************************************************/

#include "../logFormatPrt/log.h"
#include <pthread.h>


#define NUM_THREADS 3
#define TCOUNT      10
#define COUNT_LIMIT 10

int count = 0;
int thread_ids[3] = {0, 1, 2};
pthread_mutex_t count_mutex;
pthread_cond_t  count_threshold_cv;

void *inc_count(void *idp);
void *watch_count(void *idp);

int main (int argc, char *argv[])
{
    int i, rc;
    pthread_t threads[3];
    pthread_attr_t attr;

    /* Initialize mutex and condition variable objects */
    pthread_mutex_init(&count_mutex, NULL);
    pthread_cond_init (&count_threshold_cv, NULL);

    /* For portability, explicitly create threads in a joinable state */
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&threads[0], &attr, inc_count,   (void *)&thread_ids[0]);
    pthread_create(&threads[1], &attr, inc_count,   (void *)&thread_ids[1]);
    pthread_create(&threads[2], &attr, watch_count, (void *)&thread_ids[2]);

    /* Wait for all threads to complete */
    for (i = 0; i < NUM_THREADS; i++) 
    {
        pthread_join(threads[i], NULL);
    }
    printf ("Main(): Waited on %d  threads. Done.\n", NUM_THREADS);

    /* Clean up and exit */
    pthread_attr_destroy(&attr);
    pthread_mutex_destroy(&count_mutex);
    pthread_cond_destroy(&count_threshold_cv);
    pthread_exit(NULL);

    return 0;
}

void *inc_count(void *idp)
{
    int i = 0;
    int taskid = 0;
    int *my_id = (int*)idp;

    for (i = 0; i < TCOUNT; i++) 
    {
        pthread_mutex_lock(&count_mutex);
        taskid = count;
        count++;

        /*
          唤醒一个阻塞在该条件变量到线程
          如果没有线程被阻塞在条件变量上，那么调用pthread_cond_signal()将没有作用
        */
        pthread_cond_signal(&count_threshold_cv);

        printf("inc_count(): thread %d, count = %d, unlocking mutex\n", *my_id, count);
        pthread_mutex_unlock(&count_mutex);
        sleep(1);
    }
    printf("inc_count(): thread %d, Threshold reached.\n", *my_id);

    pthread_exit(NULL);
}

void *watch_count(void *idp)
{
    int *my_id = (int*)idp;
    warnf("Starting watch_count(): thread %d\n", *my_id);

    pthread_mutex_lock(&count_mutex);
    while(count < COUNT_LIMIT) 
    {
        sleep(3);
        /*
          函数将自动/原子到解锁mutex参数指向的互斥锁，并使当前线程阻塞在cv参数指向的条件变量上
          被阻塞的线程可以被pthread_cond_signal函数，pthread_cond_broadcast函数唤醒，也可能在被信号中断后被唤醒
          pthread_cond_wait函数的返回并不意味着条件的值一定发生了变化，必须重新检查条件的值.
          本例子中使用类COUNT_LIMIT最为满足条件的值
          pthread_cond_wait函数返回时，相应的互斥锁将被当前线程锁定，即使是函数出错返回
        */
        pthread_cond_wait(&count_threshold_cv, &count_mutex);
        tracef("watch_count(): thread %d Condition signal received.\n", *my_id);
    }

    count += 100;
    tracef("count has been +100 !, count = %d\n", count);
    pthread_mutex_unlock(&count_mutex);
    pthread_exit(NULL);
}

