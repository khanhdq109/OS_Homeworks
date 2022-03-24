#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "common_threads.h"

typedef struct __barrier_t {
    int n; //number of threads
    int count;
    sem_t sem_mutex;
    sem_t sem_barrier;
} barrier_t;

barrier_t b;

void barrier_init(barrier_t *barrier, int num_threads) {
    barrier->n = num_threads;
    barrier->count = 0;
    sem_init(&barrier->sem_mutex, 0, (unsigned int) 1);
    sem_init(&barrier->sem_barrier, 0, (unsigned int) 1);
}

void barrier(barrier_t *barrier) {
    sem_wait(&barrier->sem_mutex);
    barrier->count += 1;
    sem_post(&barrier->sem_mutex);

    if(barrier->count == barrier->n) sem_post(&barrier->sem_barrier);

    sem_wait(&barrier->sem_barrier);
    sem_post(&barrier->sem_barrier);
}

typedef struct __tinfo_t {
    int thread_id;
} tinfo_t;

void *child(void *arg) {
    tinfo_t *t = (tinfo_t *) arg;
    printf("child %d: before\n", t->thread_id);
    barrier(&b);
    printf("child %d: after\n", t->thread_id);
    return NULL;
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int num_threads = atoi(argv[1]);
    assert(num_threads > 0);

    pthread_t p[num_threads];
    tinfo_t t[num_threads];

    printf("parent: begin\n");
    barrier_init(&b, num_threads);

    for (int i = 0; i < num_threads; i++) {
        t[i].thread_id = i;
        Pthread_create(&p[i], NULL, child, &t[i]);
    }

    for (int i = 0; i < num_threads; i++) Pthread_join(p[i], NULL);

    printf("parent: end\n");
    return 0;
}