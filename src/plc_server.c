#include <errno.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include "plc_conf.h"
#include "plc_handle.h"
#include "plc_queue.h"
#include "plc_server.h"

struct plc_server *plc_server;

time_t timenow;
struct tm *timenow_p;

static struct plc_server *plc_server_new(void) {
    struct plc_server *new;
    new = calloc(1, sizeof(struct plc_server));
    return new;
}

static void plc_server_destroy(struct plc_server *a) {
    if (a) {
        if (a->server_sock > 0) {
            close(a->server_sock);
        }
        free(a);
    }
    a = NULL;
}

static int plc_server_sock_create(void) {
    int sock;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        return -1;
    }

    return sock;
}

static void *plc_server_thread_recv(void *arg) {
    int ret;
    struct message *msg;

    char buf[1024];
    int len;

    while (1) {

        if (plc_server->server_status == 1) {
            len = recv(plc_server->server_sock, buf, sizeof(buf), 0);
            if (len < 0) {
                plc_server->server_status = 0;
                break;
            } else if (len == 0) {
                continue;
            } else {
                time(&timenow);
                timenow_p = localtime(&timenow);
                printf("%02d/%02d/%02d %02d:%02d:%02d recv :", (1900 + timenow_p->tm_year), (1 + timenow_p->tm_mon), timenow_p->tm_mday, timenow_p->tm_hour, timenow_p->tm_min, timenow_p->tm_sec);
                printf("tcp data,length=%d\n", len);
            }
        }
    }
}

static void *plc_server_thread_send(void *arg) {
    int ret;
    struct message *msg;

    while (1) {

        while (plc_server->server_status == 0) {
            close(plc_server->server_sock);
            plc_server->server_sock = plc_server_sock_create();
            if (connect(plc_server->server_sock, (struct sockaddr *)&gconf->server, sizeof(gconf->server)) < 0) {
                // TODO
                usleep(20000);
            } else {
                plc_server->server_status = 1;
            }
        }

        while (plc_server->server_status && ((msg = (struct message *)plc_queue_dequeue(plc_server->queue)) != NULL)) {
            ret = send(plc_server->server_sock, msg->data, msg->length, 0);
            if (ret < 0) {
                plc_server->server_status = 0;
            }
            plc_message_free(msg);
            usleep(10000);
        }

        pthread_mutex_lock(&plc_server->mutex);
        pthread_cond_wait(&plc_server->cond, &plc_server->mutex);
        pthread_mutex_unlock(&plc_server->mutex);
    }
}

int plc_server_direct_forward(struct message *msg) {
    int ret;
    struct message *new;

    if (msg == NULL) {
        return -1;
    }

    new = plc_message_new(COMPONENT_PROCESS);
    if (new == NULL) {
        return -1;
    }

    plc_message_content_append(new, msg->data, msg->length);
    new->valid = 1;

    ret = plc_queue_enqueue(plc_server->queue, new);
    if (ret < 0) {
        plc_message_free(new);
        return -1;
    } else {
        pthread_mutex_lock(&plc_server->mutex);
        pthread_cond_signal(&plc_server->cond);
        pthread_mutex_unlock(&plc_server->mutex);
    }

    return 0;
}

// static void signal_handle() {
//     close(plc_server->conn);
// }

int plc_server_init(void) {
    int ret;

    /**
     * SIG_DFL: default action
     * SIG_IGN：ignore action
     * SIG_ERR: error action
     * void signal_handle(): self define action
     */
    signal(SIGPIPE, SIG_IGN);

    plc_server = plc_server_new();
    if (plc_server) {
        plc_server->queue = plc_queue_create(PLC_QUEUE_ARRAY_SIZE);
        if (plc_server->queue == NULL) {
            return -1;
        }
        plc_server->server_sock = plc_server_sock_create();

        ret = pthread_cond_init(&plc_server->cond, NULL);
        if (ret != 0) {
            return -1;
        }
        ret = pthread_mutex_init(&plc_server->mutex, NULL);
        if (ret != 0) {
            return -1;
        }
    } else {
        return -1;
    }

    return 0;
}

int plc_server_start(void) {
    int ret;

    ret = pthread_create(&plc_server->recv_tid, NULL, plc_server_thread_recv, NULL);
    if (ret != 0) {
        return -1;
    }
    ret = pthread_create(&plc_server->send_tid, NULL, plc_server_thread_send, NULL);
    if (ret != 0) {
        return -1;
    }

    return 0;
}

int plc_server_wait(void) {
    if (plc_server) {
        pthread_join(plc_server->recv_tid, NULL);
        pthread_join(plc_server->send_tid, NULL);
    }
    return 0;
}
