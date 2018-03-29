#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#include "plc_message.h"

struct message *plc_message_new(int creator) {
    struct message *msg;
    msg = calloc(1, sizeof(struct message));
    if (msg) {
        msg->creator = creator;
        msg->owner = creator;
        msg->payload = &msg->data[0];
        msg->length = 0;
        msg->valid = 0;
    }
    return msg;
}

void plc_message_free(struct message *msg) {
    if (msg == NULL) {
        return;
    }
    free(msg);
    msg = NULL;
}

int plc_message_content_append(struct message *msg, char *content, int length) {
    if (msg->length + length > PLC_MESSAGE_LENGTH_MAX) {
        return -1;
    }

    memcpy(msg->payload, content, length);
    msg->payload += length;
    msg->length += length;

    return 0;
}
