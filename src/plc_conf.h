#ifndef _PLC_CONF_H
#define _PLC_CONF_H

#include <netinet/in.h>
/* Definiations. */ //MAXPATHLEN
#define PLC_CONF_PATH_LENGTH_MAX 128
#define PLC_CONF_ABSTRACT_NUM_MAX 16

#define PLC_CONF_SENDCOMMAND_MAXLENGTH 128
#define PLC_CONF_SENDCOMMAND_MAX 32

#define PLC_UDP_PORT_DEFAULT 9600
#define PLC_UDPLOCAL_PORT_DEFAULT 20800

#define PLC_CONFUDP_PORT_DEFAULT 20888

#define PLC_SERVER_PORT_DEFAULT 10005
#define PLC_SERVERLOCAL_PORT_DEFAULT 10006

#define PLC_SENDR_INTERVAL_DEFAULT 500

/* Structures. */

typedef enum {
    PRODUCT_OMRON = 0x00,
} product_type;
typedef enum {
    SERIES_CJ2M = 0x00,
} series_type;

struct plccommand {
    unsigned int sendtimes;
    unsigned char infinite;
    unsigned char cmdlen;
    unsigned char sendcmd[PLC_CONF_SENDCOMMAND_MAXLENGTH];
};

struct sendcommand {
    unsigned char product_type; // 0: OMRON
    unsigned char series_type;  // 0: CJ2M

    char r_addr[4];       // plc device addr
    unsigned char r_unit; // 0 - F
    unsigned char r_node; // 1 - 254

    char s_addr[4];       // pc device addr
    unsigned char s_unit; // 0 - F
    unsigned char s_node; // 1 - 254

    unsigned char read_write; // 0: read   1: write
    unsigned char bit_handle; // 0: no     1:yes
    unsigned char bit_addr;   // 0 - 15

    unsigned char memory_area;  // eg: 82
    unsigned char aisle_num[2]; // eg: 0064
    unsigned char data_num[2];  // eg: 0005

    unsigned char write_data[256];

    unsigned char commandnum;
    unsigned char commnadindex;
    unsigned char generatecmd; // Whether the command has been generated using the options
    struct plccommand final_sendcmd[PLC_CONF_SENDCOMMAND_MAX];
};

struct conf {
    /* Required parameters */
    struct sockaddr_in plcudp;
    struct sockaddr_in server;

    unsigned int sendinterval;

    /* Log parameters. */
    unsigned char level;
    char logfile[PLC_CONF_PATH_LENGTH_MAX + 1];
};

/* Export prototypes. */
int plc_conf_init(void);
int plc_conf_load(char *, char *);
int plc_conf_check();

/* Export variables. */
extern struct conf *gconf;
extern struct sendcommand *sendcommand;

#endif