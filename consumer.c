// consumer.c
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#include "shared.h"

static void die(const char* msg){ perror(msg); exit(EXIT_FAILURE); }

int main(void) {
    // Open existing shared memory (start producer first)
    int fd = shm_open(SHM_NAME, O_RDWR, 0666);
    if (fd == -1) die("shm_open (start producer first)");

    ring_buffer_t* rb = mmap(NULL, sizeof(ring_buffer_t),
                             PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (rb == MAP_FAILED) die("mmap");

    // Attach to the named semaphores created by producer
    sem_t* sem_empty = sem_open(SEM_EMPTY, 0);
    if (sem_empty == SEM_FAILED) die("sem_open empty");
    sem_t* sem_full  = sem_open(SEM_FULL,  0);
    if (sem_full == SEM_FAILED) die("sem_open full");
    sem_t* sem_mutex = sem_open(SEM_MUTEX, 0);
    if (sem_mutex == SEM_FAILED) die("sem_open mutex");

    // Consume until producer marks done and buffer is drained
    while (1) {
        sem_wait(sem_full);
        sem_wait(sem_mutex);

        if (rb->count == 0 && rb->done) {  // nothing left and producer finished
            sem_post(sem_mutex);
            break;
        }

        if (rb->count > 0) {
            int item = rb->buf[rb->out];
            rb->out = (rb->out + 1) % BUFFER_CAP;
            rb->count--;
            printf("   [CONSUMER] consumed %d (count=%zu)\n", item, rb->count);
            fflush(stdout);
            sem_post(sem_mutex);
            sem_post(sem_empty);
        } else {
            sem_post(sem_mutex);
        }
    }

    printf("   [CONSUMER] finished.\n");

    // Clean up (unlink once so reruns start clean)
    munmap(rb, sizeof(ring_buffer_t));
    close(fd);

    sem_unlink(SEM_EMPTY);
    sem_unlink(SEM_FULL);
    sem_unlink(SEM_MUTEX);
    shm_unlink(SHM_NAME);
    return 0;
}
