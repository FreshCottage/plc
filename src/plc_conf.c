#include <arpa/inet.h>
#include <assert.h>
#include <netpacket/packet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <unistd.h>

#include "plc_cmd.h"
#include "plc_conf.h"
#include "plc_sendcmd.h"

struct conf *gconf;
struct sendcommand *sendcommand;

int plc_conf_read(FILE *fp) {
    if (fp == NULL) {
        return -1;
    }
    char line[256];
    memset(line, 0, sizeof(line));
    int ln;
    for (ln = 1;; ln++) {
        if (fgets(line, sizeof(line), fp) == NULL) {
            return 0;
        }
        if (plc_cmd_line(line) < 0) {
            return -1;
        }
    }

    return 0;
}

int plc_conf_load(char *config_file, char *config_default) {
    char cwd[MAXPATHLEN];
    char tmp[MAXPATHLEN + 1];
    FILE *fp;
    char *fullpath;

    if (config_file != NULL) {
        if (config_file[0] != '/') {
            getcwd(cwd, MAXPATHLEN);
            memset(tmp, 0, sizeof(tmp));
            sprintf(tmp, "%s/%s", cwd, config_file);
            fullpath = tmp;
        } else {
            fullpath = config_file;
        }

        fp = fopen(fullpath, "r");
        if (fp == NULL) {
            perror("config file in /etc/config");
            return -1;
        }
    } else {
        fp = fopen(config_default, "r");
        if (fp == NULL) {
            return -1;
        } else {
            fullpath = config_default;
        }
    }

    plc_conf_read(fp);

    fclose(fp);
    return 0;
}

int plc_command_check(void) {
    int i, j;

    printf("********** Send Command Check **********\n");

    for (i = 0; i < sendcommand->commandnum; i++) {
        printf("command%d\t", i);
        for (j = 0; j < sendcommand->final_sendcmd[i].cmdlen; j++) {
            printf("%02X", sendcommand->final_sendcmd[i].sendcmd[j]);
        }
        printf("\tinfinite=%d\tsendtime=%d\n", sendcommand->final_sendcmd[i].infinite, sendcommand->final_sendcmd[i].sendtimes);
    }

    printf("****************************************\n");

    return 0;
}

int plc_conf_check(void) {

    printf("********** Configuration **********\n");
    printf("logfile:     \t%s\n", gconf->logfile);
    printf("plcudp:      \t%s\t%u\n", inet_ntoa(gconf->plcudp.sin_addr), ntohs(gconf->plcudp.sin_port));
    printf("server:      \t%s\t%u\n", inet_ntoa(gconf->server.sin_addr), ntohs(gconf->server.sin_port));
    printf("sendinterval:\t%u(ms)\n", gconf->sendinterval / 1000);
    printf("***********************************\n");

    return 0;
}

int plc_conf_init() {
    gconf = calloc(1, sizeof(struct conf));
    if (gconf == NULL) {
        return -1;
    }
    memset(gconf, 0, sizeof(gconf));

    sendcommand = calloc(1, sizeof(struct sendcommand));
    if (sendcommand == NULL) {
        return -1;
    }
    memset(sendcommand, 0, sizeof(sendcommand));

    return 0;
}
