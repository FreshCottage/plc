#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "plc_conf.h"
#include "plc_handle.h"

int plc_configure_raddr(unsigned char *p) {
    unsigned char i;
    for (i = 0; i < 4; i++) {
        if (p != NULL && *(p + i) < 256 && *(p + i) > 0) {
            // TODO
        } else {
            return -1;
        }
    }
    memcpy(sendcommand->r_addr, p, 4);
    return 0;
}

int plc_configure_runit(unsigned char unit) {

    if (unit <= 15 && unit >= 0) {
        sendcommand->r_unit = unit;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_rnode(unsigned char node) {

    if (node <= 255 && node >= 0) {
        sendcommand->r_node = node;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_saddr(unsigned char *p) {
    unsigned char i;
    for (i = 0; i < 4; i++) {
        if (p != NULL && *(p + i) < 256 && *(p + i) > 0) {
            // TODO
        } else {
            return -1;
        }
    }
    memcpy(sendcommand->s_addr, p, 4);
    return 0;
}

int plc_configure_sunit(unsigned char unit) {

    if (unit <= 15 && unit >= 0) {
        sendcommand->s_unit = unit;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_snode(unsigned char node) {

    if (node <= 255 && node >= 0) {
        sendcommand->s_node = node;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_memory_area(unsigned char memory_area) {

    if (memory_area <= 255 && memory_area >= 0) {
        sendcommand->memory_area = memory_area;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_aisle_num(unsigned short aisle_num) {

    if (aisle_num <= 65535 && aisle_num >= 0) {
        sendcommand->aisle_num[0] = (unsigned short)aisle_num / 256;
        sendcommand->aisle_num[1] = (unsigned short)aisle_num % 256;
        return 0;
    } else {
        return -1;
    }
}

int plc_configure_handle(unsigned char read_write, unsigned char bit_handle, unsigned char bit_addr, unsigned short data_num, unsigned char *write_data, unsigned char datalen) {

    if (read_write == 0) { // 0: read
        sendcommand->read_write = read_write;
        sendcommand->data_num[0] = (unsigned short)data_num / 256;
        sendcommand->data_num[1] = (unsigned short)data_num % 256;
        return 0;
    } else if (read_write == 1) { // 1: write
        sendcommand->read_write = read_write;
        sendcommand->bit_handle = bit_handle;
        sendcommand->bit_addr = bit_addr;

        sendcommand->data_num[0] = (unsigned short)data_num / 256;
        sendcommand->data_num[1] = (unsigned short)data_num % 256;

        if (bit_addr == 0) { // 0: no     1:yes
            data_num *= 2;
        }
        if (data_num == datalen) {
            memcpy(sendcommand->write_data, write_data, datalen);
        } else {
            return -1;
        }
        return 0;
    } else {
        return -1;
    }
}