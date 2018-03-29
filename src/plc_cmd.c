#include <arpa/inet.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>

#include "plc_cmd.h"
#include "plc_conf.h"

int del_single_quote(char *src, char *des) {
    unsigned char len = strlen(src);
    if (len < 2 || *src != '\'' || *(src + len - 1) != '\'') {
        return -1;
    }
    memcpy(des, (src + 1), (len - 2));
    *(des + len - 2) = '\0';
    return 0;
}

unsigned char hextodec(char c) {
    unsigned char ret;
    if (c >= '0' && c <= '9') {
        ret = c - 48;
    } else if (c >= 'A' && c <= 'F') {
        ret = c - 65 + 10;
    } else if (c >= 'a' && c <= 'f') {
        ret = c - 97 + 10;
    } else {
        ret = 0;
    }
    return ret;
}

unsigned int ahextodec(char *p) {
    unsigned int ret;
    unsigned char len = strlen(p);
    if (len == 1) {
        ret = (unsigned int)hextodec(*p);
    } else if (len == 2) {
        ret = (unsigned int)(hextodec(*p) * 16 + hextodec(*(p + 1)));
    } else if (len == 3) {
        ret = (unsigned int)(hextodec(*p) * 256 + hextodec(*(p + 1)) * 16 + hextodec(*(p + 2)));
    } else if (len == 4) {
        ret = (unsigned int)(hextodec(*p) * 4096 + hextodec(*(p + 1)) * 256 + hextodec(*(p + 2)) * 16 + hextodec(*(p + 3)));
    } else {
        return 0;
    }
    return ret;
}

int plc_cmd_logpath(char *tok, char *usage) {
    char ret;
    char tmp[128];

    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    strcpy(gconf->logfile, tmp);
    return 0;
}

int plc_cmd_loglevel(char *tok, char *usage) {
    char ret;
    char tmp[128];

    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    gconf->level = atoi(tmp);
    return 0;
}

int plc_cmd_serverip(char *tok, char *usage) {

    char ret;
    char tmp[128];
    unsigned char addr[4];

    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (inet_pton(AF_INET, tmp, addr) != 1) {
        return -1;
    }

    gconf->server.sin_family = AF_INET;
    memcpy(&gconf->server.sin_addr, addr, sizeof(struct in_addr));

    return 0;
}

int plc_cmd_serverport(char *tok, char *usage) {
    char ret;
    char tmp[128];

    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        gconf->server.sin_port = htons(PLC_SERVER_PORT_DEFAULT);
    } else {
        gconf->server.sin_port = htons(atoi(tmp));
    }
    return 0;
}

int plc_cmd_udpip(char *tok, char *usage) {

    char ret;
    char tmp[128];
    unsigned char addr[4];

    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (inet_pton(AF_INET, tmp, addr) != 1) {
        return -1;
    }

    gconf->plcudp.sin_family = AF_INET;
    memcpy(&gconf->plcudp.sin_addr, addr, sizeof(struct in_addr));

    return 0;
}

int plc_cmd_udpport(char *tok, char *usage) {
    char ret;
    char tmp[128];

    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        gconf->plcudp.sin_port = htons(PLC_UDP_PORT_DEFAULT);
    } else {
        gconf->plcudp.sin_port = htons(atoi(tmp));
    }
    return 0;
}

int plc_cmd_interval(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        gconf->sendinterval = PLC_SENDR_INTERVAL_DEFAULT * 1000;
    } else {
        gconf->sendinterval = atoi(tmp) * 1000;
    }
    return 0;
}

int plc_cmd_plctype(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->product_type = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_plcseries(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->product_type = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_plcip(char *tok, char *usage) {
    char ret;
    char tmp[128];
    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (inet_pton(AF_INET, tmp, sendcommand->r_addr) != 1) {
        return -1;
    }
    return 0;
}

int plc_cmd_plcunit(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->r_unit = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_plcnode(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->r_node = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_pcip(char *tok, char *usage) {
    char ret;
    char tmp[128];
    if (strtok(NULL, " \t") != NULL)
        return -1;
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (inet_pton(AF_INET, tmp, sendcommand->s_addr) != 1) {
        return -1;
    }
    return 0;
}

int plc_cmd_pcunit(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->s_unit = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_pcnode(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->s_node = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_rorw(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->read_write = (unsigned char)atoi(tmp);
    }
    return 0;
}

int plc_cmd_bithandle(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->bit_handle = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_bitaddr(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->bit_addr = (unsigned char)atoi(tmp);
    }
    return 0;
}

int plc_cmd_memory(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        sendcommand->memory_area = (unsigned char)ahextodec(tmp);
    }
    return 0;
}

int plc_cmd_aisle(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        unsigned short aisle = (unsigned short)ahextodec(tmp);
        sendcommand->aisle_num[0] = (unsigned char)(aisle >> 8);
        sendcommand->aisle_num[1] = (unsigned char)(aisle & 0x00FF);
    }
    return 0;
}

int plc_cmd_handlenum(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        unsigned short data_num = (unsigned short)ahextodec(tmp);
        sendcommand->data_num[0] = (unsigned char)(data_num >> 8);
        sendcommand->data_num[1] = (unsigned char)(data_num & 0x00FF);
    }
    return 0;
}

int plc_cmd_wdata(char *tok, char *usage) {
    char ret;
    char tmp[128];
    ret = del_single_quote(tok, tmp);
    if (ret < 0)
        return -1;
    if (strtok(NULL, " \t") != NULL) {
        return -1;
    } else {
        unsigned char len = strlen(tmp);
        if ((len & 0x01) != 0) {
            return -1;
        }
        unsigned char i;
        unsigned char data_len = 0;
        for (i = 0; i < len; i += 2) {
            sendcommand->write_data[data_len++] = (unsigned int)(hextodec(*(tmp + i)) * 16 + hextodec(*(tmp + i + 1)));
        }
        int writenum = sendcommand->data_num[0] * 256 + sendcommand->data_num[1];
        if (!sendcommand->bit_handle) {
            writenum *= 2;
        }
        if (data_len != writenum) {
            return -1;
        }
    }
    return 0;
}

struct cmd cmd[] =
    {
        {"logfilepath", 11, plc_cmd_logpath, "logfilepath"},
        {"level", 5, plc_cmd_loglevel, "loglevel"},
        {"serverip", 8, plc_cmd_serverip, "serverip"},
        {"serverport", 10, plc_cmd_serverport, "serverport"},
        {"udpip", 5, plc_cmd_udpip, "udpip"},
        {"udpport", 7, plc_cmd_udpport, "udpport"},
        {"interval", 8, plc_cmd_interval, "intervel"},
        {"plctype", 7, plc_cmd_plctype, "plctype"},
        {"plcseries", 9, plc_cmd_plcseries, "plcseries"},
        {"plcip", 5, plc_cmd_plcip, "plcip"},
        {"plcunit", 7, plc_cmd_plcunit, "plcunit"},
        {"plcnode", 7, plc_cmd_plcnode, "plcnode"},
        {"pcip", 4, plc_cmd_pcip, "pcip"},
        {"pcunit", 6, plc_cmd_pcunit, "pcunit"},
        {"pcnode", 6, plc_cmd_pcnode, "pcnode"},
        {"rorw", 4, plc_cmd_rorw, "read or write"},
        {"bithandle", 9, plc_cmd_bithandle, "bithandle"},
        {"bitaddr", 7, plc_cmd_bitaddr, "bitaddr"},
        {"memory", 6, plc_cmd_memory, "memory"},
        {"aisle", 5, plc_cmd_aisle, "aisle"},
        {"handlenum", 9, plc_cmd_handlenum, "handlenum"},
        {"wdata", 5, plc_cmd_wdata, "wdata"},
        {NULL, 0, NULL, ""}};

struct cmd defaultcmd[] =
    {
        {NULL, 0, NULL, ""}};

int plc_cmd_dispatch(char *tok, struct cmd *cmd, char *root) {
    struct cmd *tmp;
    tok = strtok(NULL, " \t");
    for (tmp = cmd; tmp->name != NULL; tmp++) {
        if (strncmp(tok, tmp->name, tmp->len) == 0) {
            tok = strtok(NULL, " \t");
            if (tok == NULL) {
                return -1;
            }
            return tmp->func(tok, tmp->usage);
        }
    }

    return -1;
}

int plc_cmd_line(char *line) {
    char *l, *tok;

    l = line;
    while ((line[strlen(l) - 1] == '\n') || (line[strlen(l) - 1] == '\r')) {
        line[strlen(l) - 1] = '\0';
    }

    tok = strtok(l, " \t");
    if (tok == NULL) {
        return 0;
    }

    switch (*tok) {
    case 'o':
        return plc_cmd_dispatch(tok, cmd, NULL);
    default:
        return 0;
    }
    return 0;
}