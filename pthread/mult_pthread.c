/*
 * 多线程的实现
 * Date: 2015.06.25
 * */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUM_THREAD 6

void *thread_function(void *arg);

int main()
{
	int res;
	pthread_t a_thread[NUM_THREAD];	
	
	void *thread_result;
	int lots_of_threads;
	for(lots_of_threads = 0; lots_of_threads < NUM_THREAD; lots_of_threads++)
	{
		res = pthread_create(&(a_thread[lots_of_threads]), NULL, thread_function, (void*)&lots_of_threads);
		if(res != 0)
		{
			perror("pthread_create");
			exit(EXIT_FAILURE);
		}
		sleep(2);
	}

	printf("waiting for the thread quit\n");
	
	for(lots_of_threads = NUM_THREAD - 1; lots_of_threads >= 0;lots_of_threads--)
	{
		res = pthread_join(a_thread[lots_of_threads],&thread_result);
		if(res == 0)
		{
			printf("a thread end, and return : %s\n",(char *)thread_result);
		}
		else
		{
			perror("pthread_join");
		}
	}

	printf("all the threads end\n");

	return 0;
}

void *thread_function(void *arg)
{
	int my_number = *(int *)arg;
	int rand_num;
	printf("the thread_function is running, the parameter is %d\n", my_number);
	rand_num = 1 + (int)(9.0 * rand()) / (RAND_MAX + 1.0);

	sleep(rand_num);
	printf("the %d thread end\n", my_number);
	pthread_exit("I'm finish");
}

