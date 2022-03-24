#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <pthread.h>

#define ONE_MILLION 1000000
#define MAXHEAP_SIZE 16
#define MAX_THREAD 8

typedef struct __elem_heap_t
{
    int value;
    pthread_mutex_t lock;
} elem_heap_t;

typedef struct __maxheap_arr_t
{
    elem_heap_t *arr;
    int size;
    int capacity;
    pthread_mutex_t lock;
} maxheap_arr_t;

typedef struct __myarg_t
{
    int *start;
    int *end;
} myarg_t;

static maxheap_arr_t heap_obj;

static int init_max_heap(maxheap_arr_t *heap_obj, unsigned int init_capacity)
{
    if (init_capacity <= 0)
        return 0;
    heap_obj->arr = malloc(sizeof *(heap_obj->arr) * init_capacity);
    for (int i = 0; i < heap_obj->capacity; i++)
    {
        heap_obj->arr[i].value = 0;
        pthread_mutex_init(&heap_obj->arr[i].lock, NULL);
    }
    heap_obj->capacity = init_capacity;
    heap_obj->size = 0;
    pthread_mutex_init(&heap_obj->lock, NULL);
    return 1;
}

static int push(maxheap_arr_t *heap_obj, int val)
{
    pthread_mutex_lock(&heap_obj->lock);
    if (heap_obj->size == heap_obj->capacity)
    {
        pthread_mutex_unlock(&heap_obj->lock);
        return 0;
    }
    int tmp, i = heap_obj->size;
    pthread_mutex_lock(&heap_obj->arr[i].lock);
    heap_obj->arr[i].value = val;
    if (++heap_obj->size == 1)
    {
        pthread_mutex_unlock(&heap_obj->arr[i].lock);
        pthread_mutex_unlock(&heap_obj->lock);
        return 1;
    }
    pthread_mutex_unlock(&heap_obj->lock);
    pthread_mutex_lock(&heap_obj->arr[(int)((i - 1) / 2)].lock);
    while (i != 0 && heap_obj->arr[(int)((i - 1) / 2)].value < heap_obj->arr[i].value)
    {
        tmp = heap_obj->arr[i].value;
        heap_obj->arr[i].value = heap_obj->arr[(int)((i - 1) / 2)].value;
        heap_obj->arr[(int)((i - 1) / 2)].value = tmp;
        pthread_mutex_unlock(&heap_obj->arr[i].lock);
        i = (int)((i - 1) / 2);
        if (i != 0)
            pthread_mutex_lock(&heap_obj->arr[(int)((i - 1) / 2)].lock);
    }
    pthread_mutex_unlock(&heap_obj->arr[i].lock);
    if (i != 0)
        pthread_mutex_unlock(&heap_obj->arr[(int)((i - 1) / 2)].lock);
    return 1;
}

static int destroy_max_heap(maxheap_arr_t *heap_obj)
{
    if (heap_obj->arr == NULL)
        return 0;
    for (int i = 0; i < heap_obj->capacity; i++)
        pthread_mutex_destroy(&heap_obj->arr[i].lock);
    free(heap_obj->arr);
    heap_obj->arr = NULL;
    heap_obj->size = 0;
    heap_obj->arr = 0;
    heap_obj->capacity = 0;
    pthread_mutex_destroy(&heap_obj->lock);
    return 1;
}

static void *thread_function(void *args)
{
    myarg_t *m = (myarg_t *)args;
    for (int *i = m->start; i < m->end; i += 1)
        push(&heap_obj, *i);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    int num_arr[MAXHEAP_SIZE] = {31, 36, 42, 51, 34, 43, 65, 16, 2, 6, 98, 81, 80, 29, 45, 66};
    init_max_heap(&heap_obj, MAXHEAP_SIZE);
    for (int i = 1; i <= MAX_THREAD; i += 1)
    {
        myarg_t *args = malloc((size_t)i * sizeof(myarg_t));
        int j;
        for (j = 0; j < i; j++)
        {
            args[j].start = &num_arr[(int)(j * MAXHEAP_SIZE / i)];
            args[j].end = &num_arr[(int)((j + 1) * MAXHEAP_SIZE / i)];
        }
        pthread_t *threads = malloc((size_t)i * sizeof(pthread_t));
        struct timeval start, end;

        /*************************
        BEGIN MULTI-THREAD SECTION (PUSH)
        *************************/
        gettimeofday(&start, NULL);
        for (j = 0; j < i; j++)
            pthread_create(&threads[j], NULL, &thread_function, &args[j]);
        for (j = 0; j < i; j++)
            pthread_join(threads[j], NULL);
        gettimeofday(&end, NULL);
        /***********************
        END MULTI-THREAD SECTION (PUSH)
        ***********************/

        long long start_usec, end_usec;

        // Calculate execution time
        start_usec = start.tv_sec * ONE_MILLION + start.tv_usec;
        end_usec = end.tv_sec * ONE_MILLION + end.tv_usec;
        for (j = 0; j < heap_obj.size; j++)
        {
            printf("%d", heap_obj.arr[j].value);
            if ((j + 1) != heap_obj.size)
                printf(" ");
            else
                printf("\n");
        }
        printf("\n%d threads\n", i);
        printf("Time (seconds): %f\n\n----------\n",
               ((double)(end_usec - start_usec) / ONE_MILLION));

        // Free up memory
        free(threads);
        free(args);
        heap_obj.size = 0;
    }
    destroy_max_heap(&heap_obj);
    return 0;
}
