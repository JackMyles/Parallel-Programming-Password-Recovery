#include "queue.h"

#include <pthread.h>
#include <stdlib.h>

typedef struct node {
    void *value;
    void *next;
} node_t;

typedef struct queue {
    node_t *head;
    node_t *tail;
    pthread_cond_t cond;
    pthread_mutex_t lock;
} queue_t;

queue_t *queue_init(void) {
    queue_t *queue = malloc(sizeof(queue_t));
    queue->head = NULL;
    queue->tail = NULL;
    pthread_mutex_init(&queue->lock, NULL);
    pthread_cond_init(&queue->cond, NULL);
    return queue;
}

void queue_enqueue(queue_t *queue, void *value) {
    pthread_mutex_lock(&queue->lock);
    node_t *node = malloc(sizeof(node_t));
    node->value = value;
    node->next = NULL;
    if (queue->head == NULL) {
        queue->head = node;
    }
    else {
        queue->tail->next = node;
    }
    queue->tail = node;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->lock);
}

void *queue_dequeue(queue_t *queue) {
    pthread_mutex_lock(&queue->lock);
    while (queue->head == NULL) {
        pthread_cond_wait(&queue->cond, &queue->lock);
    }
    node_t *head = queue->head;
    void *value = head->value;
    queue->head = head->next;
    if (queue->head == NULL) {
        queue->tail = NULL;
    }
    free(head);
    pthread_mutex_unlock(&queue->lock);
    return value;
}

void queue_free(queue_t *queue) {
    free(queue);
}
