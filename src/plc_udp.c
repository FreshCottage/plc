#include <netinet/in.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "plc_conf.h"
#include "plc_handle.h"
#include "plc_log.h"
#include "plc_process.h"
#include "plc_queue.h"
#include "plc_server.h"
#include "plc_udp.h"

struct plc_udp *plc_udp;

time_t timenow;
struct tm *timenow_p;

static struct plc_udp *plc_udp_new(void) {
    struct plc_udp *new;
    new = calloc(1, sizeof(struct plc_udp));
    return new;
}

static void plc_udp_destroy(struct plc_udp *a) {
    if (a) {
        if (a->udp_sock > 0) {
            close(a->udp_sock);
        }
        free(a);
    }
    a = NULL;
}

static int plc_udp_sock_create(void) {
    int sock;
    int ret;
    struct sockaddr_in local;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        return -1;
    }

    memset(&local, 0, sizeof(local));
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    unsigned char i = 0;
    do {
        local.sin_port = htons(PLC_UDPLOCAL_PORT_DEFAULT + i);
        ret = bind(sock, (struct sockaddr *)&local, sizeof(struct sockaddr_in));
    } while (ret < 0);

    return sock;
}

static void *plc_udp_thread_send(void *arg) {
    int ret;
    unsigned char i, j;
    struct message *msg;

    char buf_send[] = {0x80, 0x00, 0x02, 0x00, 0x03, 0x00, 0x00, 0x05, 0x00, 0x00, 0x01, 0x01, 0x82, 0x00, 0x64, 0x00, 0x00, 0x05};

    while (1) {
        if (sendcommand->commandnum) {
            for (i = 0; i < sendcommand->commandnum; i++) {
                if ((sendcommand->final_sendcmd[i].infinite == 1) || (sendcommand->final_sendcmd[i].sendtimes > 0)) {
                    ret = sendto(plc_udp->udp_sock, sendcommand->final_sendcmd[i].sendcmd, sendcommand->final_sendcmd[i].cmdlen, 0, (struct sockaddr *)&gconf->plcudp, sizeof(struct sockaddr_in));
                    if (ret < 0) {
                    } else {
                        if (sendcommand->final_sendcmd[i].sendtimes > 0) {
                            sendcommand->final_sendcmd[i].sendtimes--;
                        }
                    }
                    usleep(gconf->sendinterval);
                }
            }
        } else {
            ret = sendto(plc_udp->udp_sock, buf_send, sizeof(buf_send), 0, (struct sockaddr *)&gconf->plcudp, sizeof(struct sockaddr_in));
            if (ret < 0) {
            }
            usleep(gconf->sendinterval);
        }
    }
}

static void *plc_udp_thread_recv(void *arg) {
    struct sockaddr_in from;
    int size = sizeof(from);
    int len;
    char buf[1024];
    struct message *msg;
    int ret;

    while (1) {

        len = recvfrom(plc_udp->udp_sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &size);
        if (len <= 0) {
            continue;
        } else {
            time(&timenow);
            timenow_p = localtime(&timenow);
            printf("%02d/%02d/%02d %02d:%02d:%02d recv :", (1900 + timenow_p->tm_year), (1 + timenow_p->tm_mon), timenow_p->tm_mday, timenow_p->tm_hour, timenow_p->tm_min, timenow_p->tm_sec);
            int i;
            for (i = 0; i < len; i++) {
                printf("%02X", (unsigned char)(buf[i]));
            }
            printf(" len=%d\n", len);

            plc_log_info("Recv data and length = %d\n", len);
        }

        /* Allocate new message. */
        msg = plc_message_new(COMPONENT_UDP_RECV);
        if (msg == NULL) {
            continue;
        }
        plc_message_content_append(msg, buf, len);
        msg->valid = 1;

        ret = plc_queue_enqueue(plc_server->queue, msg);
        if (ret < 0) {
            plc_message_free(msg);
        } else {
            pthread_mutex_lock(&plc_server->mutex);
            pthread_cond_signal(&plc_server->cond);
            pthread_mutex_unlock(&plc_server->mutex);
        }
    }
}

int plc_udp_direct_forward(struct message *msg) {
    int ret;
    struct message *new;

    if (msg == NULL) {
        return -1;
    }

    new = plc_message_new(COMPONENT_UDP_SEND);
    if (new == NULL) {
        return -1;
    }

    plc_message_content_append(new, msg->data, msg->length);
    new->valid = 1;

    ret = plc_queue_enqueue(plc_udp->queue, new);
    if (ret < 0) {
        plc_message_free(new);
        return -1;
    } else {
        pthread_mutex_lock(&plc_udp->mutex);
        pthread_cond_signal(&plc_udp->cond);
        pthread_mutex_unlock(&plc_udp->mutex);
    }
    return 0;
}

int plc_udp_init(void) {
    int ret;

    plc_udp = plc_udp_new();
    if (plc_udp) {
        plc_udp->queue = plc_queue_create(PLC_QUEUE_ARRAY_SIZE);
        if (plc_udp->queue == NULL) {
            return -1;
        }
        plc_udp->udp_sock = plc_udp_sock_create();

        ret = pthread_cond_init(&plc_udp->cond, NULL);
        if (ret != 0) {
            return -1;
        }

        ret = pthread_mutex_init(&plc_udp->mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

int plc_udp_start(void) {
    int ret;

    ret = pthread_create(&plc_udp->recv_tid, NULL, plc_udp_thread_recv, NULL);
    if (ret != 0) {
        return -1;
    }

    ret = pthread_create(&plc_udp->send_tid, NULL, plc_udp_thread_send, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int plc_udp_wait(void) {
    if (plc_udp) {
        pthread_join(plc_udp->recv_tid, NULL);
        pthread_join(plc_udp->send_tid, NULL);
    }
    return 0;
}
