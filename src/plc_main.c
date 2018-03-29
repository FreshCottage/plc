#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>

#include "plc.h"
#include "plc_conf.h"
#include "plc_sendcmd.h"

/* Configuration file and directory. */
char *config_file = PLC_DEFAULT_CONFIG;

int main(int argc, char **argv) {
    int ret;

    /* Initialize the global configuration. */
    ret = plc_conf_init();
    if (ret < 0) {
        exit(-1);
    }
    /* Load the configuration. */
    ret = plc_conf_load(config_file, NULL);
    if (ret < 0) {
        exit(-1);
    }
    /* Load the command. */
    ret = plc_generate_cmd();
    if (ret < 0) {
        exit(-1);
    }
    /* Check the configuration. */
    ret = plc_conf_check();
    if (ret < 0) {
        goto stop;
    }
    /* Check the command. */
    ret = plc_command_check();
    if (ret < 0) {
        goto stop;
    }

    /* Initialization. */
    ret = plc_init();
    if (ret < 0) {
        goto stop;
    }
    /* Start the process threads. */
    ret = plc_start();
    if (ret < 0) {
        goto stop;
    }
    // Waiting for the threads end.
    ret = plc_wait();
    if (ret < 0) {
        goto stop;
    }
    /* Loop until not running. */
    while ((ret = plc_status()) == 0) {
        printf("Hello\n");
        sleep(1);
    }

stop:
    return 0;
}