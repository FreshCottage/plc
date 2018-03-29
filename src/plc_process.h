#ifndef _PLC_PROCESS_H
#define _PLC_PROCESS_H

/* Definitions. */
#define PLC_PROCESS_QUEUE_LENGTH 1024
#define PLC_PROCESS_THREAD_MAX 2

/* Structures. */
struct process {
    pthread_t tid[PLC_PROCESS_THREAD_MAX];
    pthread_cond_t cond;
    pthread_mutex_t mutex;
    struct queue *queue;
};

/* Export prototypes. */
int plc_process_init(void);
int plc_process_start(void);
int plc_process_wait(void);

/* Export variables. */
extern struct process *process;

#endif /* _IWSN_AGW_PROCESS_H */
