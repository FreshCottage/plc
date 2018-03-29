#ifndef _PLC_QUEUE_H
#define _PLC_QUEUE_H

/* Definitions. */
#define PLC_QUEUE_ARRAY_SIZE 1024
#define PLC_QUEUE_DATA_SIZE (sizeof(void *))

/* Structures */
struct queue {
    void **array;
    u_int array_size;
    u_int size;
    u_int head;
    u_int tail;
    pthread_mutex_t mutex;
};

/* Export prototypes. */
struct queue *plc_queue_create(u_int);
void plc_queue_destroy(struct queue *);
int plc_queue_enqueue(struct queue *, void *);
void *plc_queue_dequeue(struct queue *);
int plc_queue_empty(struct queue *);
int plc_queue_full(struct queue *);

#endif