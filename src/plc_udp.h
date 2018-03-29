#ifndef _PLC_UDP_H
#define _PLC_UDP_H
#include "plc_message.h"

/* Definitions. */

/* Structures. */
struct plc_udp {
    int udp_sock;
    pthread_t recv_tid;
    pthread_t send_tid;
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct queue *queue;
};

/* Export Prototypes. */
int plc_udp_init(void);
int plc_udp_start(void);
int plc_udp_wait(void);
int plc_udp_direct_forward(struct message *msg);

/* Export variables. */
extern struct plc_udp *plc_udp;

#endif
