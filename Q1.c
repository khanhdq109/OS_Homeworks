// An example of using gettimeofday() to calculate the execution
// time of a program
//
// The gettimeofday() function gets the current system's clock 
// time which is expressed in elapsed seconds and microseconds
// since <00:00:00, January 1, 1970>
//
// The smallest interval it can measure accurately is 1 microsecond

#include <sys/time.h>
#include <stdio.h>

int main() {
	struct timeval start, end;
	gettimeofday(&start, NULL);

	int counter = 0;
	for (int i = 0; i < 10000000; i++) 
		counter++;

	gettimeofday(&end, NULL);
	long dauert = (end.tv_sec * 1000000 + end.tv_usec) - (start.tv_sec * 1000000 + start.tv_usec);
	double dauert_sec = (double)dauert / 1000000.0;
	printf("Time taken to execute: %ld msec\n", dauert);
	printf("Time taken to execute: %lf sec\n", dauert_sec);
	
	return 0;
}
