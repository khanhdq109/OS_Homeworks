#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define ONE_MILLION 1000000
#define MAXHEAP_SIZE 16
#define MAX_THREAD 8

typedef struct __maxheap_arr_t {
    int *arr;
    int size;
    int capacity;
    pthread_mutex_t lock;
} maxheap_arr_t;

typedef struct __myarg_t {
    int *start;
    int *end;
} myarg_t;

static maxheap_arr_t heap_obj;

static int init_max_heap(maxheap_arr_t *heap_obj, unsigned int init_capacity) {
    if (init_capacity <= 0)
        return 0;
    heap_obj->arr = malloc(sizeof *(heap_obj->arr) * init_capacity);
    memset(heap_obj->arr, 0, init_capacity * sizeof *(heap_obj->arr));
    heap_obj->capacity = init_capacity;
    heap_obj->size = 0;
    pthread_mutex_init(&heap_obj->lock, NULL);
    return 1;
}

static int push(maxheap_arr_t *heap_obj, int val) {
    pthread_mutex_lock(&heap_obj->lock);
    if (heap_obj->size == heap_obj->capacity) {
        pthread_mutex_unlock(&heap_obj->lock);
        return 0;
    }
    int tmp, i = heap_obj->size;
    heap_obj->arr[i] = val;
    if (++heap_obj->size == 1) {
        pthread_mutex_unlock(&heap_obj->lock);
        return 1;
    }
    while (i != 0 && heap_obj->arr[(int)((i - 1) / 2)] < heap_obj->arr[i]) {
        tmp = heap_obj->arr[i];
        heap_obj->arr[i] = heap_obj->arr[(int)((i - 1) / 2)];
        heap_obj->arr[(int)((i - 1) / 2)] = tmp;
        i = (int)((i - 1) / 2);
    }
    pthread_mutex_unlock(&heap_obj->lock);
    return 1;
}

// Comment it to reduce executable file size
/*
static void pop(maxheap_arr_t *heap_obj) {
    pthread_mutex_lock(&heap_obj->lock);
    if (heap_obj->size <= 1) {
        heap_obj->size = 0;
        pthread_mutex_unlock(&heap_obj->lock);
        return;
    }
    int tmp;
    int left, right, max, parent_node = 0;
    heap_obj->arr[0] = heap_obj->arr[--heap_obj->size];
    while (1) {
        left = 2 * parent_node + 1;
        right = 2 * parent_node + 2;
        if (left >= heap_obj->size || left < 0)
            left = -1;
        if (right >= heap_obj->size || right < 0)
            right = -1;
        if (left != -1 && heap_obj->arr[left] > heap_obj->arr[parent_node])
            max = left;
        else
            max = parent_node;
        if (right != -1 && heap_obj->arr[right] > heap_obj->arr[max])
            max = right;
        if (max != parent_node) {
            tmp = heap_obj->arr[max];
            heap_obj->arr[max] = heap_obj->arr[parent_node];
            heap_obj->arr[parent_node] = tmp;
            parent_node = max;
        } else
            break;
    }
    pthread_mutex_unlock(&heap_obj->lock);
}

static int top(maxheap_arr_t *heap_obj, int *ans) {
    pthread_mutex_lock(&heap_obj->lock);
    if (heap_obj->size == 0) {
        pthread_mutex_unlock(&heap_obj->lock);
        return 0;
    }
    *ans = heap_obj->arr[0];
    pthread_mutex_unlock(&heap_obj->lock);
    return 1;
}
*/

static int destroy_max_heap(maxheap_arr_t *heap_obj) {
    if (heap_obj->arr == NULL)
        return 0;
    free(heap_obj->arr);
    heap_obj->arr = NULL;
    heap_obj->size = 0;
    heap_obj->arr = 0;
    heap_obj->capacity = 0;
    pthread_mutex_destroy(&heap_obj->lock);
    return 1;
}

static void *thread_function(void *args) {
    myarg_t *m = (myarg_t *)args;
    for (int *i = m->start; i < m->end; i += 1)
        push(&heap_obj, *i);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    int num_arr[MAXHEAP_SIZE] = {31, 36, 42, 51, 34, 43, 65, 16,
                                 2,  6,  98, 81, 80, 29, 45, 66};
    init_max_heap(&heap_obj, MAXHEAP_SIZE);
    for (int i = 1; i <= MAX_THREAD; i += 1) {
        myarg_t *args = malloc((size_t)i * sizeof(myarg_t));
        int j;
        for (j = 0; j < i; j++) {
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
        for (j = 0; j < heap_obj.size; j++) {
            printf("%d", heap_obj.arr[j]);
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
