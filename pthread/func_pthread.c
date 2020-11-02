#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

void *thread_function(void *arg);
char message[] = "THREAD_TEST";
struct test
{
	int a;
	int b;
	int c;
};
struct test outpara;

int main()
{
	int res;
	pthread_t a_thread;
	void *thread_result;
	struct test para = {
	.a = 10,
	.b = 20,
	.c = 30,
	};
	res = pthread_create(&a_thread, NULL, thread_function, (void *)&para);
	if(res != 0)
	{
		perror("Pthread_create");
		exit(EXIT_FAILURE);
	}
	printf("wait the thread %u finish\n", a_thread);
	res = pthread_join(a_thread, &thread_result);
	if(res != 0)
	{
		printf("Fail to wait finish the thread\n");
		exit(EXIT_FAILURE);
	}
	printf("the thread has finish, return %d\n", ((struct test*)thread_result)->a);

	exit(EXIT_SUCCESS);
}

void *thread_function(void *arg)
{
	struct test inpara = *(struct test*)arg;
	printf("thread is running, the parameter is %d, %d, %d\n", inpara.a,inpara.b, inpara.c);
	sleep(300);
	outpara.a = 100, 
	outpara.b = 200,
	outpara.c = 300,
	pthread_exit(&outpara);
}
