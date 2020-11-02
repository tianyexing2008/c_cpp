#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *);
pthread_mutex_t work_mutex;
#define WORK_SIZE 1024

char work_area[WORK_SIZE];

static int time_to_exit = 1;
int main22()
{
	int res;
	pthread_t a_thread;
	void *thread_result;
	
	res = pthread_mutex_init(&work_mutex, NULL);
	if(res != 0)
	{
		perror("pthread_mutex_init");
		exit(EXIT_FAILURE);
	}
	
	res = pthread_create(&a_thread, NULL, thread_function, NULL);
	if(res != 0)
	{
		perror("pthread_create");
		exit(EXIT_FAILURE);
	}
	
	pthread_mutex_lock(&work_mutex);
	printf("input the message to send,'end' to quit\n");
	while(time_to_exit)
	{
		fgets(work_area, WORK_SIZE, stdin);
		pthread_mutex_unlock(&work_mutex);
		while(1)
		{
			pthread_mutex_lock(&work_mutex);
			if(work_area[0] == '\0')
			{
				pthread_mutex_unlock(&work_mutex);
				sleep(1);
			}
			else
			{
				break;
			}
		}
	}

	pthread_mutex_unlock(&work_mutex);
	printf("wait the thread end\n");
	res = pthread_join(a_thread, &thread_result);
	if(res != 0)
	{
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	printf("the thread end\n");

	pthread_mutex_destroy(&work_mutex);

	return 0;
}


void *thread_function(void *arg)
{
	sleep(1);
	pthread_mutex_lock(&work_mutex);
	
	while(strncmp("end", work_area, 3) != 0)
	{
		printf("Receive %d char\n", strlen(work_area) - 1);
		work_area[0] = '\0';
		pthread_mutex_unlock(&work_mutex);
		sleep(1);
		
		pthread_mutex_lock(&work_mutex);
		while(work_area[0] == '\0')
		{
			pthread_mutex_unlock(&work_mutex);
			sleep(1);
			pthread_mutex_lock(&work_mutex);
		}
	}
	time_to_exit = 0;
	work_area[0] = '\0';
	pthread_mutex_unlock(&work_mutex);
	pthread_exit(0);
}
