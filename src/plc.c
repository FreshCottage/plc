#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>

#include "plc.h"
#include "plc_confudp.h"
#include "plc_log.h"
#include "plc_process.h"
#include "plc_server.h"
#include "plc_udp.h"

int plc_init(void) {
    plc_log_init();
    plc_process_init();
    plc_udp_init();
    plc_confudp_init();
    plc_server_init();
    return 0;
}

int plc_start(void) {
    plc_process_start();
    plc_udp_start();
    plc_confudp_start();
    plc_server_start();
    return 0;
}

int plc_wait(void) {
    plc_process_wait();
    plc_udp_wait();
    plc_confudp_wait();
    plc_server_wait();
    return 0;
}

int plc_status(void) {
    return 0;
}
