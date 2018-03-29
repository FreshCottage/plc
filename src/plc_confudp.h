#ifndef _PLC_CONFUDP_H
#define _PLC_CONFUDP_H

/* Definitions. */

/* Structures. */
struct plc_confudp {
    int udp_sock;
    pthread_t recv_tid;
};

/* Export Prototypes. */
int plc_confudp_init(void);
int plc_confudp_start(void);
int plc_confudp_wait(void);

/* Export variables. */

#endif
