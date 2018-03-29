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
#include "plc_confudp.h"
#include "plc_handle.h"
#include "plc_log.h"

struct plc_confudp *plc_confudp;

static struct plc_confudp *plc_confudp_new(void) {
    struct plc_confudp *new;
    new = calloc(1, sizeof(struct plc_confudp));
    return new;
}

static void plc_udp_destroy(struct plc_confudp *a) {
    if (a) {
        if (a->udp_sock > 0) {
            close(a->udp_sock);
        }
        free(a);
    }
    a = NULL;
}

static int plc_confudp_sock_create(void) {
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
    local.sin_port = htons(PLC_CONFUDP_PORT_DEFAULT);

    ret = bind(sock, (struct sockaddr *)&local, sizeof(struct sockaddr_in));
    if (ret < 0) {
        perror("bind");
        exit(1);
    }

    return sock;
}

static void *plc_confudp_thread_recv(void *arg) {
    struct sockaddr_in from;
    socklen_t fromlen;
    int len;
    unsigned char buf[1024];

    while (1) {

        memset(&from, 0, sizeof(struct sockaddr_in));
        fromlen = sizeof(struct sockaddr_in);

        len = recvfrom(plc_confudp->udp_sock, buf, sizeof(buf), 0, (struct sockaddr *)&from, &fromlen);
        if (len <= 0) {
            continue;
        } else {
            int i = 0;
            printf("configure udp recv data : ");
            for (i = 0; i < len; i++) {
                printf("%02X", buf[i]);
            }
            printf("\n");
            // TODO configure plc options
        }
        usleep(20000);
    }
}

int plc_confudp_init(void) {
    int ret;

    plc_confudp = plc_confudp_new();
    if (plc_confudp) {
        plc_confudp->udp_sock = plc_confudp_sock_create();
        if (plc_confudp->udp_sock < 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

int plc_confudp_start(void) {
    int ret;

    ret = pthread_create(&plc_confudp->recv_tid, NULL, plc_confudp_thread_recv, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int plc_confudp_wait(void) {
    if (plc_confudp) {
        pthread_join(plc_confudp->recv_tid, NULL);
    }
    return 0;
}
