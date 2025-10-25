#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <semaphore.h>

#include "shared.h"

static void die(const char* msg){ perror(msg); exit(EXIT_FAILURE); }

int main(void) {
    int fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    if (fd == -1 && errno == EEXIST) {
        shm_unlink(SHM_NAME);
        fd = shm_open(SHM_NAME, O_CREAT | O_EXCL | O_RDWR, 0666);
    }
    if (fd == -1) die("shm_open");
    if (ftruncate(fd, sizeof(ring_buffer_t)) == -1) die("ftruncate");

        ring_buffer_t* rb = mmap(NULL, sizeof(ring_buffer_t),
        PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (rb == MAP_FAILED) die("mmap");

    rb->in = rb->out = rb->count = 0;
    rb->done = false;
        sem_t* sem_empty = sem_open(SEM_EMPTY, O_CREAT | O_EXCL, 0666, BUFFER_CAP);
    if (sem_empty == SEM_FAILED && errno == EEXIST) {
        sem_unlink(SEM_EMPTY);
        sem_empty = sem_open(SEM_EMPTY, O_CREAT | O_EXCL, 0666, BUFFER_CAP);
    }
    if (sem_empty == SEM_FAILED) die("sem_open empty");

    sem_t* sem_full  = sem_open(SEM_FULL,  O_CREAT | O_EXCL, 0666, 0);
    if (sem_full == SEM_FAILED && errno == EEXIST) {
        sem_unlink(SEM_FULL);
        sem_full = sem_open(SEM_FULL, O_CREAT | O_EXCL, 0666, 0);
    }
    if (sem_full == SEM_FAILED) die("sem_open full");

    sem_t* sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    if (sem_mutex == SEM_FAILED && errno == EEXIST) {
        sem_unlink(SEM_MUTEX);
        sem_mutex = sem_open(SEM_MUTEX, O_CREAT | O_EXCL, 0666, 1);
    }
    if (sem_mutex == SEM_FAILED) die("sem_open mutex");
    srand((unsigned)time(NULL));
    for (int item = 0; item < PRODUCE_COUNT; ++item) {
        sem_wait(sem_empty);
        sem_wait(sem_mutex);

        rb->buf[rb->in] = item;
        rb->in = (rb->in + 1) % BUFFER_CAP;
        rb->count++;
        printf("[PRODUCER] produced %d (count=%zu)\n", item, rb->count);
        fflush(stdout);

        sem_post(sem_mutex);
        sem_post(sem_full);

        usleep((rand() % 300 + 100) * 1000); // small delay so you see interleaving

            sem_wait(sem_mutex);
    rb->done = true;
    sem_post(sem_mutex);
    sem_post(sem_full);  // wake consumer if it’s waiting

    munmap(rb, sizeof(ring_buffer_t));
    close(fd);

    printf("[PRODUCER] finished.\n");
    return 0;
  }

    }
