#ifndef _PLC_LOG_H
#define _PLC_LOG_H

/* Definitions. */
#define PLC_LOG_MODULE_NAME_MAX 32
#define PLC_LOG_BUFFER_SIZE 1024 * 1024 * 4

typedef enum log_level {
    LOG_ERROR = 1,
    LOG_WARNING = 2,
    LOG_NOTICE = 3,
    LOG_INFORMATION = 4,
    LOG_DEBUG = 5,
} log_level_t;

#define plc_log_err(fmt, arg...)        \
    do {                                \
        plc_log(LOG_ERROR, fmt, ##arg); \
    } while (0)

#define plc_log_warn(fmt, arg...)         \
    do {                                  \
        plc_log(LOG_WARNING, fmt, ##arg); \
    } while (0)

#define plc_log_notice(fmt, arg...)      \
    do {                                 \
        plc_log(LOG_NOTICE, fmt, ##arg); \
    } while (0)

#define plc_log_info(fmt, arg...)             \
    do {                                      \
        plc_log(LOG_INFORMATION, fmt, ##arg); \
    } while (0)

#define plc_log_debug(fmt, arg...)      \
    do {                                \
        plc_log(LOG_DEBUG, fmt, ##arg); \
    } while (0)

/* Structures. */
struct plclog {
    log_level_t level;
    FILE *fp;
    char logdest[MAXPATHLEN + 1];
    pthread_mutex_t mutex;
};

/* Export prototypes. */
int plc_log_init(void);
int plc_log(log_level_t, const char *format, ...);

#endif
