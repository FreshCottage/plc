#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "plc_queue.h"

struct queue *plc_queue_create(u_int size) {
    struct queue *q;
    q = calloc(1, sizeof(struct queue));
    if (q == NULL) {
        return NULL;
    }

    q->array = calloc(1, PLC_QUEUE_DATA_SIZE * size);
    q->array_size = size;
    q->size = 0;

    q->head = q->tail = 0;

    if (pthread_mutex_init(&q->mutex, NULL) != 0) {
        free(q->array);
        q->array = NULL;
        free(q);
        q = NULL;
        return NULL;
    }
    return q;
}

void plc_queue_destroy(struct queue *q) {
    if (q != NULL) {
        pthread_mutex_destroy(&q->mutex);
        free(q->array);
        q->array = NULL;
        free(q);
        q = NULL;
    }
}

int plc_queue_enqueue(struct queue *q, void *data) {
    if (plc_queue_full(q)) {
        return -1;
    }
    pthread_mutex_lock(&q->mutex);

    q->array[q->tail] = data;
    q->tail = (q->tail + 1) % q->array_size;
    q->size++;

    pthread_mutex_unlock(&q->mutex);
    return 0;
}

void *plc_queue_dequeue(struct queue *q) {
    void *data;
    if (plc_queue_empty(q)) {
        return NULL;
    }
    pthread_mutex_lock(&q->mutex);

    data = q->array[q->head];
    q->array[q->head] = NULL;
    q->head = (q->head + 1) % q->array_size;
    q->size--;

    pthread_mutex_unlock(&q->mutex);
    return data;
}

int plc_queue_empty(struct queue *q) {
    int empty_flag;
    pthread_mutex_lock(&q->mutex);

    empty_flag = q->head == q->tail;

    pthread_mutex_unlock(&q->mutex);
    return empty_flag;
}

int plc_queue_full(struct queue *q) {
    int full_flag;
    pthread_mutex_lock(&q->mutex);

    full_flag = q->tail == (q->array_size - 1 + q->head) % q->array_size;

    pthread_mutex_unlock(&q->mutex);
    return full_flag;
}
