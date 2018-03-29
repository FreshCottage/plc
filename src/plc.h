#ifndef _PLC_H
#define _PLC_H

/* Default configuration file name. */
#define PLC_DEFAULT_CONFIG "/etc/config/plcconf"

#define PLC_VERSION "1.0"
#define PLC_COPYRIGHT "Copyright 2017-present, Beijing Jiaotong University."
#define PLC_BUG_ADDRESS "16120042@bjtu.edu.cn"

/* Export Prototypes. */
int plc_init(void);
int plc_start(void);
int plc_wait(void);
int plc_status(void);

#endif