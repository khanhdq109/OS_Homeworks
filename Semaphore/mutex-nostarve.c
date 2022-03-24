#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "common_threads.h"

//
// Here, you have to write (almost) ALL the code. Oh no!
// How can you show that a thread does not starve
// when attempting to acquire this mutex you build?
//

typedef struct __ns_mutex_t {
    	int room1, room2;
	sem_t lock;
	sem_t s1, s2;
} ns_mutex_t;

void ns_mutex_init(ns_mutex_t *m) {
    	m->room1 = 0;
	m->room2 = 0;
	sem_init(&m->lock, 0, 1);
	sem_init(&m->s1, 0, 1);
	sem_init(&m->s2, 0, 0);
}

void ns_mutex_acquire(ns_mutex_t *m) {
    	sem_wait(&m->lock);
	m->room1 ++;
	sem_post(&m->lock);
	sem_wait(&m->s1);
	m->room2 ++;
	sem_wait(&m->lock);
	m->room1 --;
	if (m->room1) {
		sem_post(&m->lock);
		sem_post(&m->s1);
	} else {
		sem_post(&m->lock);
		sem_post(&m->s2);
	}
	sem_wait(&m->s2);
	m->room2 --;
}

void ns_mutex_release(ns_mutex_t *m) {
    if (m->room2) {
		sem_post(&m->s2);
	} else {
		sem_post(&m->s1);
	}
}


void *worker(void *arg) {
    return NULL;
}

int value = 0;

ns_mutex_t lock1;
ns_mutex_t lock2;


int main(int argc, char *argv[]) {
    printf("parent: begin\n");
    
    ns_mutex_init(&lock1);
    ns_mutex_init(&lock2);  
    ns_mutex_acquire(&lock1);
    printf("read %d\n", value);
    ns_mutex_acquire(&lock2);
    value++;
    printf("write %d\n", value);
    ns_mutex_release(&lock1);
    ns_mutex_release(&lock2);
    printf("end %d\n", value);
    
    
    printf("parent: end\n");
    return 0;
}

