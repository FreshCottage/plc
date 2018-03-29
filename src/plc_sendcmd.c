#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "plc_conf.h"
#include "plc_sendcmd.h"

int plc_generate_cmd() {

    int len = 0;
    unsigned char buf[128];
    memset(buf, 0, sizeof(buf));
    if (sendcommand->product_type == PRODUCT_OMRON) {
        if (sendcommand->series_type == SERIES_CJ2M) {
            buf[len++] = 0x80;
            buf[len++] = 0x00;
            buf[len++] = 0x02;

            buf[len++] = 0x00;
            buf[len++] = (unsigned char)sendcommand->r_addr[3];
            buf[len++] = sendcommand->r_unit;

            buf[len++] = 0x00;
            buf[len++] = (unsigned char)sendcommand->s_addr[3];
            buf[len++] = sendcommand->s_unit;

            buf[len++] = 0x00;

            buf[len++] = 0x01;
            buf[len++] = sendcommand->read_write + 1;
            buf[len++] = sendcommand->memory_area;
            buf[len++] = sendcommand->aisle_num[0];
            buf[len++] = sendcommand->aisle_num[1];

            if (sendcommand->bit_handle) {
                buf[len++] = sendcommand->bit_addr;
            } else {
                buf[len++] = 0x00;
            }

            buf[len++] = sendcommand->data_num[0];
            buf[len++] = sendcommand->data_num[1];

            if (sendcommand->read_write == 1) { //write data
                int i;
                int writenum = sendcommand->data_num[0] * 256 + sendcommand->data_num[1];
                if (!sendcommand->bit_handle) {
                    writenum *= 2;
                }
                for (i = 0; i < writenum; i++) {
                    buf[len++] = sendcommand->write_data[i];
                }

                sendcommand->final_sendcmd[sendcommand->commandnum].infinite = 0;
                sendcommand->final_sendcmd[sendcommand->commandnum].sendtimes = 1;
            } else {
                sendcommand->final_sendcmd[sendcommand->commandnum].infinite = 1; //read data
            }
            memcpy(sendcommand->final_sendcmd[sendcommand->commandnum].sendcmd, buf, len);

            sendcommand->final_sendcmd[sendcommand->commandnum].cmdlen = len;
            if (sendcommand->generatecmd == 0) {
                sendcommand->commandnum++;
            }
            sendcommand->generatecmd = 1;

            return 0;
        }
    }

    return -1;
}