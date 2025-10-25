#ifndef SHARED_H
#define SHARED_H

#include <semaphore.h>
#include <stdbool.h>   

// Named objects for POSIX shared memory & semaphores
#define SHM_NAME   "/pc_shm_michael"
#define SEM_EMPTY  "/pc_sem_empty_michael"
#define SEM_FULL   "/pc_sem_full_michael"
#define SEM_MUTEX  "/pc_sem_mutex_michael"
#define BUFFER_CAP 2
#define PRODUCE_COUNT 20

typedef struct {
    int    buf[BUFFER_CAP];
    size_t in;
    size_t out;
    size_t count;
    bool   done;         
} ring_buffer_t;

#endif
