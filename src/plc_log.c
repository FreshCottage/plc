#include <netinet/in.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/param.h>
#include <unistd.h>

#include "plc_conf.h"
#include "plc_log.h"
#include "plc_queue.h"

struct plclog *plclog;

static struct plclog *plc_log_new(void) {
    struct plclog *new;
    new = calloc(1, sizeof(struct plclog));
    return new;
}

static int plc_log_level_check(log_level_t level) {
    if (level > plclog->level) {
        return -1;
    }
    return 0;
}

static const char *plc_log_level_to_string(log_level_t level) {
    switch (level) {
    case LOG_ERROR:
        return "ERROR";
    case LOG_WARNING:
        return "WARN";
    case LOG_NOTICE:
        return "NOTICE";
    case LOG_INFORMATION:
        return "INFO";
    case LOG_DEBUG:
        return "DEBUG";
    default:
        return "UNKNOWN";
    }
}

static int plc_log_pre_make(char *buffer, log_level_t level) {
    struct timeval stamp;
    struct tm *timenow;

    gettimeofday(&stamp, NULL);
    timenow = localtime(&stamp.tv_sec);

    return snprintf(buffer, PLC_LOG_BUFFER_SIZE, "%04d-%02d-%02d %02d:%02d:%02d.%06ld [%s]: ", timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec, stamp.tv_usec, plc_log_level_to_string(level));
}

static char *plc_log_file_time(char *filename) {
    struct timeval stamp;
    struct tm *timenow;

    gettimeofday(&stamp, NULL);
    timenow = localtime(&stamp.tv_sec);
    //snprintf(filename + strlen (filename), PLC_CONF_PATH_LENGTH_MAX, "_%04d-%02d-%02d_%02d:%02d:%02d.log",
    //         timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday, timenow->tm_hour, timenow->tm_min, timenow->tm_sec);
    snprintf(filename + strlen(filename), PLC_CONF_PATH_LENGTH_MAX, "-%04d%02d%02d.log",
             timenow->tm_year + 1900, timenow->tm_mon + 1, timenow->tm_mday);
    return filename;
}

static int plc_log_open(void) {
    if (plclog) {
        if (plclog->logdest[0] == '\0') {
            plclog->fp = stdout;
        } else {
            plclog->fp = fopen(plc_log_file_time(plclog->logdest), "a");
            if (plclog->fp == NULL) {
                return -1;
            }
        }
        setvbuf(plclog->fp, (char *)NULL, _IOLBF, 0);
    }

    return 0;
}

static void plc_log_close(void) {
    if (plclog && plclog->fp != NULL) {
        fflush(plclog->fp);
        fclose(plclog->fp);
        plclog->fp = NULL;
    }
}

static int plc_log_write(char *buffer, int length) {
    if (access(plclog->logdest, W_OK) != 0) {
        pthread_mutex_lock(&plclog->mutex);
        plc_log_close();
        plc_log_open();
        pthread_mutex_unlock(&plclog->mutex);
    }

    if (fwrite(buffer, length, 1, plclog->fp) == 1) {
        fflush(plclog->fp);
        *buffer = '\0';
    } else {
        fprintf(stderr, "Failed to write to logfile: %s", buffer);
        return -1;
    }

    return 0;
}

int plc_log_init(void) {
    int ret;
    char cwd[PLC_CONF_PATH_LENGTH_MAX + 1];

    plclog = plc_log_new();
    if (plclog == NULL) {
        return -1;
    }

    plclog->level = gconf->level;
    if (gconf->logfile[0] != '/') {
        getcwd(cwd, PLC_CONF_PATH_LENGTH_MAX);
        sprintf(plclog->logdest, "%s/%s", cwd, gconf->logfile);
    } else {
        sprintf(plclog->logdest, "%s", gconf->logfile);
    }

    pthread_mutex_init(&plclog->mutex, NULL);

    ret = plc_log_open();
    if (ret < 0) {
        return -1;
    }

    return 0;
}

int plc_log(log_level_t level, const char *format, ...) {
    int ret;
    char logbuf[PLC_LOG_BUFFER_SIZE];
    char *start;
    int prelen;
    int size;

    ret = plc_log_level_check(level);
    if (ret < 0) {
        return -1;
    }

    start = logbuf;

    prelen = plc_log_pre_make(start, level);
    start += prelen;

    va_list args;
    va_start(args, format);
    size = vsnprintf(start, PLC_LOG_BUFFER_SIZE - prelen, format, args);
    va_end(args);

    if (plclog && plclog->fp != NULL) {
        plc_log_write(logbuf, prelen + size);
    } else {
        fprintf(stderr, "%s", logbuf);
    }

    return 0;
}
