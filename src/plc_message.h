#ifndef _PLC_MESSAGE_H
#define _PLC_MESSAGE_H

/* Definitions. */
#define PLC_MESSAGE_LENGTH_MAX 256

/* Structures. */
struct message {
    int creator;
    int owner;
    int valid;
    char *payload;
    int length;
    char data[PLC_MESSAGE_LENGTH_MAX];
};

typedef enum {
    COMPONENT_NULL = 0x00,
    COMPONENT_QUEUE = 0x01,
    COMPONENT_PROCESS = 0x02,
    COMPONENT_UDP_RECV = 0x05,
    COMPONENT_UDP_SEND = 0x06,
    COMPONENT_SERVER_RECV = 0x07,
    COMPONENT_SERVER_SEND = 0x08,
} plc_component_enum_t;

/* Export prototypes. */
struct message *plc_message_new(int);
void plc_message_free(struct message *);
int plc_message_content_append(struct message *, char *, int);

#endif