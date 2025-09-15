#include "thread_pooler.h"

void *do_task (void *arg){

    thread_pool_t *pool = (thread_pool_t *)arg;

    while (1) {

        pthread_mutex_lock(&pool->lock);

        while (pool->count == 0 && !pool->shutdown) {
            pthread_cond_wait(&pool->notify, &pool->lock);
        }

        if (pool->shutdown) {
            pthread_mutex_unlock(&pool->lock);
            pthread_exit(NULL);
        }

        task_t task = pool->queue[pool->head];

        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;

        pthread_mutex_unlock(&pool->lock);

        (*(task.function))(task.arg);

    }

    return NULL;
}

int thread_pool_init(thread_pool_t *pool, int queue_size){

   if (pool == NULL || queue_size <= 0) return -1;

    pool->queue_size = queue_size;
    pool->queue = (task_t *)malloc(sizeof(task_t) * queue_size);
    if (!pool->queue) return -1;

    pool->head = pool->tail = pool->count = 0;
    pool->shutdown = 0;

    if (pthread_mutex_init(&pool->lock, NULL) != 0 ||
        pthread_cond_init(&pool->notify, NULL) != 0) {
        free(pool->queue);
        return -1;
    }

    int i;
    for (i = 0; i < MAX_THREADS; i++) {
        if (pthread_create(&pool->threads[i], NULL, do_task, (void *)pool) != 0) {
            perror("pthread_create failed, cleaning up");

            pool->shutdown = 1;
            pthread_cond_broadcast(&pool->notify);
            for (int j = 0; j < i; j++) {
                pthread_join(pool->threads[j], NULL);
            }

            pthread_mutex_destroy(&pool->lock);
            pthread_cond_destroy(&pool->notify);
            free(pool->queue);
            return -1;
        }
    }

    return 0;

}

int thread_pool_add(thread_pool_t *pool, void (*function)(void *), void *arg){

     if (!pool || !function) return -1;

    pthread_mutex_lock(&pool->lock);

    if (pool->count == pool->queue_size) {
        pthread_mutex_unlock(&pool->lock);
        return -1;
    }

    pool->queue[pool->tail].function = function;
    pool->queue[pool->tail].arg = arg;
    pool->tail = (pool->tail + 1) % pool->queue_size;
    pool->count++;

    pthread_cond_signal(&pool->notify);
    pthread_mutex_unlock(&pool->lock);
    return 0;
    
}


int thread_pool_destroy(thread_pool_t *pool) {
    if (!pool) return -1;

    pthread_mutex_lock(&pool->lock);
    pool->shutdown = 1;
    pthread_cond_broadcast(&pool->notify);
    pthread_mutex_unlock(&pool->lock);

    for (int i = 0; i < MAX_THREADS; i++) {
        pthread_join(pool->threads[i], NULL);
    }

    pthread_mutex_lock(&pool->lock);
    while (pool->count > 0) {
        task_t task = pool->queue[pool->head];
        pool->head = (pool->head + 1) % pool->queue_size;
        pool->count--;
        if (task.arg) {
            free(task.arg); 
        }
    }
    pthread_mutex_unlock(&pool->lock);

    pthread_mutex_destroy(&pool->lock);
    pthread_cond_destroy(&pool->notify);
    free(pool->queue);

    return 0;
}