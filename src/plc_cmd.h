#ifndef _PLC_CMD_H
#define _PLC_CMD_H

/* Command structure. */
struct cmd {
    char *name;
    unsigned char len;
    int (*func)(char *line, char *usage);
    char *usage;
};

/* Export prototypes. */
int plc_cmd_line(char *);

#endif