#ifndef _PLC_SERVER_H
#define _PLC_SERVER_H

#include "plc_message.h"

/* Structures. */
struct plc_server {
    int server_sock;
    int server_status;

    pthread_t recv_tid;
    pthread_t send_tid;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct queue *queue;
};

/* Export prototypes. */
int plc_server_init(void);
int plc_server_start(void);
int plc_server_wait(void);
int plc_server_direct_forward(struct message *msg);

/* Export variables. */
extern struct plc_server *plc_server;

#endif