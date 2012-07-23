#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <time.h>

#define TEST_COUNT 1000

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  cond = PTHREAD_COND_INITIALIZER;

int i = 1;

#if 0
/*
 * @brief 线程休眠指定的毫秒数
 *        此函数是可重入函数，可以并行多次调用
 *        TODO: 此函数有问题，执行468次后无法加锁
 */
void thread_sleep(const long ms)
{
	pthread_cond_t *cond;
	pthread_mutex_t *mutex;
	struct timeval now;
	struct timespec outtime;
	long sec, usec;

	mutex = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	if (!mutex)
	{
		perror("fail to malloc mutex!\n");
		goto _end;
	}
	pthread_mutex_init(mutex, NULL);

	cond = (pthread_cond_t*)malloc(sizeof(pthread_cond_t));
	if (!cond)
	{
		perror("fail to malloc cond!\n");
		goto _end1;
	}
	pthread_cond_init(cond, NULL);

	printf("sleep lock!\n");
	pthread_mutex_lock(mutex);
	printf("sleep lock OK!\n");

	// calc time difference
	sec = (long)(ms/1000);
	usec = (long)(ms%1000);

	printf("sec = %ld, usec = %ld\n", sec, usec);

	// get current time
	gettimeofday(&now, NULL);

	// set timeout
	outtime.tv_sec = now.tv_sec + sec;
	outtime.tv_nsec = now.tv_usec + usec * 1000;

	// just wait timeout
	pthread_cond_timedwait(cond, mutex, &outtime);

	pthread_mutex_unlock(mutex);

	// release resources
	free(cond);
_end1:
	free(mutex);
_end:
	return;
}
#endif

void *thread2(void *junk)
{  
	pthread_mutex_lock(&mutex); //互斥锁  

	printf("---------------------------------\n");
	printf("thread2: id %lu\n", pthread_self());
	printf("thread2: send signal to thread1...\n");

	if(!pthread_cond_signal(&cond))
		printf("thread2: send signal ok!\n");
	else
		perror("thread2: send signal fail!\n");

	pthread_mutex_unlock(&mutex);  
	//sleep(1);  
	return junk;
} 

void *thread1(void* junk)
{
	int i;
	pthread_t tmp_id;

	for (i=0; i<TEST_COUNT; i++)
	{
		pthread_mutex_lock(&mutex);

		printf("---------------- %d ------------------\n", i+1);
		printf("thread1: create new thread...\n");
		if (!pthread_create(&tmp_id, NULL, thread2, NULL))
		{
			printf("thread1: OK, id is %lu\n", (unsigned long)tmp_id);
			printf("thread1: waiting new thread cond...\n");
			if(!pthread_cond_wait(&cond,&mutex))
				printf("thread1: recv cond ok!\n");
			else
				perror("thread1: recv cond fail!\n");
		}
		else
		{
			perror("thread1: create failed!\n");
		}

		// sleep 500ms
		struct timespec to;
		to.tv_sec = 0;
		to.tv_nsec = 1000 * 1000 * 500;
		nanosleep(&to, NULL);

		//thread_sleep(500);

		pthread_mutex_unlock(&mutex);
	}

	return junk;
}

int main()
{ 
	pthread_t thread1_id;

	if (pthread_create(&thread1_id, NULL, thread1, NULL))
	{
		perror("create thread1:");
		return -1;
	}

	if (pthread_join(thread1_id, NULL))
	{
		perror("join thread1 error:");
		return -1;
	}

	return 0;
}
