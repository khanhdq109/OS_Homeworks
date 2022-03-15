#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define NUMCPUS 	6
#define LIMIT 		1000000

int NUMTHREADS = 1;
int threshold = 1024;
int global = 0;
int local[NUMCPUS];
pthread_mutex_t glock;
pthread_mutex_t llock[NUMCPUS];

void * updates(void * arg) {
	int tid = * (int *)arg;
	int cpu = tid % NUMCPUS;

	pthread_mutex_lock(&llock[cpu]);

	for (int i = 0; i < LIMIT; i++) {
		local[cpu] = local[cpu] + 1;
		if (local[cpu] >= threshold) {
			pthread_mutex_lock(&glock);
			global = global + local[cpu];
			pthread_mutex_unlock(&glock);
			local[cpu] = 0;
		}
	}

	pthread_mutex_lock(&glock);
	global = global + local[cpu];
	pthread_mutex_unlock(&glock);
	local[cpu] = 0;

	pthread_mutex_unlock(&llock[cpu]);

	pthread_exit(NULL);
}

int get_global() {
	pthread_mutex_lock(&glock);
	int ret = global;
	pthread_mutex_unlock(&glock);
	return ret;
}

int main(int argc, char ** argv) {
	if (argc == 2) 
		NUMTHREADS = atoi(argv[1]);
	else if (argc == 3) {
		NUMTHREADS = atoi(argv[1]);
		threshold = atoi(argv[2]);
	}

	struct timeval start, end;
	gettimeofday(&start, NULL);

	pthread_mutex_init(&glock, NULL);
	for (int i = 0; i < NUMCPUS; i++) {
		pthread_mutex_init(&llock[i], NULL);
		local[i] = 0;
	}

	pthread_t p[NUMTHREADS];
	int tid[NUMTHREADS];

	for (int i = 0; i < NUMTHREADS; i++) {
		tid[i] = i;
		pthread_create(&p[i], NULL, updates, &tid[i]);
	}

	for (int i = 0; i < NUMTHREADS; i++)
		pthread_join(p[i], NULL);

	printf("counter = %d\n", global);

	pthread_mutex_destroy(&glock);
	for (int i = 0; i < NUMCPUS; i++)
		pthread_mutex_destroy(&llock[i]);

	gettimeofday(&end, NULL);
	long dauert = (end.tv_sec * 1000000 + end.tv_usec)
		- (start.tv_sec * 1000000 + start.tv_usec);
	double dauert_sec = (double)dauert / 1000000;
	printf("Execution time: %lf sec\n", dauert_sec);

	return 0;
}
