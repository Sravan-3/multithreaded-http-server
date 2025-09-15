#ifndef THREAD_POOLER_H
#define THREAD_POOLER_H

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MAX_THREADS 4

typedef struct
{
    void (*function) (void *args);
    void *arg;   
} task_t;

typedef struct {
    pthread_mutex_t lock;
    pthread_cond_t notify;
    pthread_t threads[MAX_THREADS];

    task_t *queue;
    int queue_size;
    int head;
    int tail;
    int count;

    int shutdown;
} thread_pool_t;

int thread_pool_init(thread_pool_t *pool, int queue_size);
int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *arg);
int thread_pool_destroy(thread_pool_t *pool);

#endif