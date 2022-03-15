#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>
#include <string.h>

#define NUMCPUS	 	6
#define LIMIT		1000000

int counter = 0;
int NUMTHREADS = 1;
pthread_mutex_t lock;

void increment() {
	pthread_mutex_lock(&lock);
	counter = counter + 1;
	pthread_mutex_unlock(&lock);
}

void decrement() {
	pthread_mutex_lock(&lock);
	counter = counter - 1;
	pthread_mutex_unlock(&lock);
}

void * updates(void * arg) {
	// pthread_mutex_lock(&lock);
	for (int i = 0; i < LIMIT; i++) {
		// counter = counter + 1;
		increment();
	}
	// pthread_mutex_unlock(&lock);
	pthread_exit(NULL);
}

int main(int argc, char ** argv) {
	if (argc > 1) 
		NUMTHREADS = atoi(argv[1]);

	struct timeval start, end;
	gettimeofday(&start, NULL);

	pthread_mutex_init(&lock, NULL);

	pthread_t p[NUMTHREADS];

	for (int i = 0; i < NUMTHREADS; i++)
		pthread_create(&p[i], NULL, updates, NULL);
	
	for (int i = 0; i < NUMTHREADS; i++)
		pthread_join(p[i], NULL);

	printf("counter = %d\n", counter);

	pthread_mutex_destroy(&lock);
	
	gettimeofday(&end, NULL);
	long dauert = (end.tv_sec * 1000000 + end.tv_usec)
		- (start.tv_sec * 1000000 + start.tv_usec);
	double dauert_sec = (double)dauert / 1000000;
	printf("Execution time: %lf sec\n", dauert_sec);

	return 0;
}
