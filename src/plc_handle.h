#ifndef _PLC_HANDLE_H
#define _PLC_HANDLE_H

/* Definitions. */

/* Structures. */

/* Export prototypes. */
int plc_configure_raddr(unsigned char *);
int plc_configure_runit(unsigned char);
int plc_configure_rnode(unsigned char);
int plc_configure_saddr(unsigned char *);
int plc_configure_sunit(unsigned char);
int plc_configure_snode(unsigned char);
int plc_configure_memory_area(unsigned char);
int plc_configure_aisle_num(unsigned short);
int plc_configure_handle(unsigned char, unsigned char, unsigned char, unsigned short, unsigned char *, unsigned char);

#endif
