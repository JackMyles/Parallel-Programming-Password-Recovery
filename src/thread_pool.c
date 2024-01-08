#include "thread_pool.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct thread_pool {
    pthread_t *threads;
    queue_t *queue;
    size_t size;
} thread_pool_t;

typedef struct work {
    work_function_t work_func;
    void *aux;
} work_t;

void *thread_work(void *ptr) {
    queue_t *queue = (queue_t *) ptr;
    while (true) {
        work_t *work = queue_dequeue(queue);
        if (work == NULL) {
            return NULL;
        }
        (work->work_func)(work->aux);
        free(work);
    }
}

thread_pool_t *thread_pool_init(size_t num_worker_threads) {
    thread_pool_t *thread_pool = malloc(sizeof(thread_pool_t));
    thread_pool->threads = malloc(sizeof(pthread_t) * num_worker_threads);
    thread_pool->queue = queue_init();
    thread_pool->size = num_worker_threads;
    for (size_t i = 0; i < num_worker_threads; i++) {
        pthread_create(&thread_pool->threads[i], NULL, thread_work, thread_pool->queue);
    }
    return thread_pool;
}

void thread_pool_add_work(thread_pool_t *pool, work_function_t function, void *aux) {
    work_t *work = malloc(sizeof(work_t));
    work->work_func = function;
    work->aux = aux;
    queue_enqueue(pool->queue, (void *) work);
}

void thread_pool_finish(thread_pool_t *pool) {
    for (size_t i = 0; i < pool->size; i++) {
        queue_enqueue(pool->queue, NULL);
    }
    for (size_t i = 0; i < pool->size; i++) {
        pthread_join(pool->threads[i], NULL);
    }
    queue_free(pool->queue);
    free(pool->threads);
    free(pool);
}
