#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "plc_handle.h"
#include "plc_message.h"
#include "plc_process.h"
#include "plc_queue.h"
#include "plc_server.h"

struct process *process;

static struct process *plc_process_new(void) {
    struct process *new;
    new = calloc(1, sizeof(struct process));
    return new;
}

static void plc_process_destroy(struct process *p) {
    if (p) {
        free(p);
    }
    p = NULL;
}

static void *plc_process(void *arg) {
    int ret;
    struct message *msg;

    while (1) {
        while ((msg = (struct message *)plc_queue_dequeue(process->queue)) != NULL) {
            printf("Process handle messge:");
            int i;
            for (i = 0; i < msg->length; i++) {
                printf("%02X", (unsigned char)msg->data[i]);
            }
            printf("\n");
            plc_server_direct_forward(msg);
            plc_message_free(msg);
        }

        pthread_mutex_lock(&process->mutex);
        pthread_cond_wait(&process->cond, &process->mutex);
        pthread_mutex_unlock(&process->mutex);

        usleep(10000);
    }
}

int plc_process_init(void) {
    int ret;

    process = plc_process_new();
    if (process) {
        process->queue = plc_queue_create(PLC_PROCESS_QUEUE_LENGTH);
        if (process->queue == NULL) {
            plc_process_destroy(process);
            return -1;
        }

        ret = pthread_cond_init(&process->cond, NULL);
        if (ret != 0) {
            return -1;
        }
        ret = pthread_mutex_init(&process->mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

int plc_process_start(void) {
    int i;
    int ret;
    for (i = 0; i < PLC_PROCESS_THREAD_MAX; i++) {
        ret = pthread_create(&process->tid[i], NULL, plc_process, NULL);
        if (ret != 0) {
            return -1;
        }
    }
    return 0;
}

int plc_process_wait(void) {
    int i;
    for (i = 0; i < PLC_PROCESS_THREAD_MAX; i++) {
        pthread_join(process->tid[i], NULL);
    }
    return 0;
}
