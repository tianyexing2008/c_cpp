#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>

void *thread_function(void *);

sem_t bin_sem;
#define WORK_SIZE 1024
char work_area[WORK_SIZE];

int main()
{
	int res;
	pthread_t a_thread;
	void *thread_result;
	res = sem_init(&bin_sem, 0, 0);	
	if(res != 0)
	{
		perror("sem_init");
		exit(EXIT_FAILURE);
	}

	res = pthread_create(&a_thread, NULL, thread_function, NULL);
	if(res != 0)
	{
		perror("pthrad_create");
		exit(EXIT_FAILURE);
	}
	printf("input the message you want to send,'end' to quit\n");
	while(strncmp("end", work_area, 3) != 0)
	{
		fgets(work_area, WORK_SIZE, stdin);
		sem_post(&bin_sem);
	}

	printf("wait to the thread end\n");
	res = pthread_join(a_thread, &thread_result);
	if(res != 0)
	{
		perror("pthread_join");
		exit(EXIT_FAILURE);
	}
	printf("the thread end\n");
	sem_destroy(&bin_sem);
	
	return 0;
}

void *thread_function(void *arg)
{
	sem_wait(&bin_sem);
	while(strncmp("end", work_area, 3) != 0)
	{
		printf("Receive %d char\n", strlen(work_area) -1);
		sem_wait(&bin_sem);
	}
	sleep(6);
	pthread_exit(NULL);
}


