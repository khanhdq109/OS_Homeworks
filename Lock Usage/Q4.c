#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#define ONE_MILLION 1000000

typedef struct __node_t {
    struct __node_t *next;
    pthread_mutex_t lock;
    int key;
} node_t;

typedef struct __linked_list_t {
    node_t *head;
    pthread_mutex_t insert_lock;
} list_t;

static void LList_Init(list_t *l_list) {
    l_list->head = NULL;
    pthread_mutex_init(&l_list->insert_lock, NULL);
}

// Insert node at [head]
static void LList_Insert(list_t *l_list, int key) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->key = key;
    pthread_mutex_init(&new_node->lock, NULL);
    pthread_mutex_lock(&l_list->insert_lock);
    new_node->next = l_list->head;
    l_list->head = new_node;
    pthread_mutex_unlock(&l_list->insert_lock);
}

static int LList_Lookup(list_t *l_list, int key) {
    int ans = 0;
    node_t *curr = l_list->head;
    if (!curr)
        return ans;
    pthread_mutex_lock(&curr->lock);
    while (curr) {
        if (curr->key == key) {
            ans = 1;
            pthread_mutex_unlock(&curr->lock);
            break;
        }
        pthread_mutex_t *tempLock = &curr->lock;
        curr = curr->next;
        if (curr)
            pthread_mutex_lock(&curr->lock);
        pthread_mutex_unlock(tempLock);
    }
    return ans;
}

static void LList_Print(list_t *l_list) {
    node_t *curr = l_list->head;
    if (!curr)
        return;
    pthread_mutex_lock(&curr->lock);
    while (curr) {
        printf("%d\n", curr->key);
        pthread_mutex_t *tempLock = &curr->lock;
        curr = curr->next;
        if (curr)
            pthread_mutex_lock(&curr->lock);
        pthread_mutex_unlock(tempLock);
    }
}

static void LList_Free(list_t *l_list) {
    if (!l_list->head)
        return;
    node_t *tempNode = NULL;
    pthread_mutex_lock(&l_list->head->lock);
    while (l_list->head) {
        tempNode = l_list->head;
        l_list->head = l_list->head->next;
        if (l_list->head)
            pthread_mutex_lock(&l_list->head->lock);
        pthread_mutex_unlock(&tempNode->lock);
        pthread_mutex_destroy(&tempNode->lock);
        free(tempNode);
    }
    pthread_mutex_destroy(&l_list->insert_lock);
    free(l_list);
}

static void *thread_function(void *args) {
    list_t *l = (list_t *)args;
    LList_Lookup(l, 2);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr,
                "usage: ./Q4 <number_of_threads> <print_list_or_not>\n");
        exit(EXIT_FAILURE);
    }
    int thread_count = atoi(argv[1]);
    int is_print = atoi(argv[2]);
    FILE *fptr;
    fptr = fopen("./data.txt", "r");
    if (fptr == NULL) {
        fprintf(stderr, "Missing \"./data.txt\" file\n");
        exit(EXIT_FAILURE);
    }
    int list_length = 0;
    int buf = 0;
    list_t *list = malloc(sizeof(list_t));
    LList_Init(list);
    while (fscanf(fptr, "%d", &buf) != EOF) {
        LList_Insert(list, buf);
        ++list_length;
    }
    fclose(fptr);
    for (int i = 1; i <= thread_count; i++) {
        int s = 0;
        struct timeval start, end;
        s = gettimeofday(&start, NULL);
        pthread_t *threads = malloc((size_t)i * sizeof(pthread_t));
        for (int j = 0; j < i; j++)
            pthread_create(&threads[j], NULL, &thread_function, list);
        for (int k = 0; k < i; k++)
            pthread_join(threads[k], NULL);
        s = gettimeofday(&end, NULL);
        long long startusec, endusec;
        startusec = start.tv_sec * ONE_MILLION + start.tv_usec;
        endusec = end.tv_sec * ONE_MILLION + end.tv_usec;
        if (is_print)
            LList_Print(list);
        printf("%d threads, time (seconds): %f\n\n", i,
               ((double)(endusec - startusec) / ONE_MILLION));
        free(threads);
    }
    LList_Free(list);
    return 0;
}