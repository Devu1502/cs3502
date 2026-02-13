/*
Name: Devananda Sreekanth
Course: CS 3502
Section: W03
Assignment: A2 Bounded Buffer
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <semaphore.h>
#include <fcntl.h>
#include "buffer.h"
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <consumer_id> <num_items>\n", argv[0]);
        return 1;
    }

    int consumer_id = atoi(argv[1]);
    int num_items = atoi(argv[2]);

    printf("Consumer %d will consume %d items\n", consumer_id, num_items);

    int shm_id = shmget(SHM_KEY, sizeof(shared_buffer_t), 0666);
    if (shm_id < 0) {
        perror("shmget failed");
        return 1;
    }

    shared_buffer_t *buffer = (shared_buffer_t *)shmat(shm_id, NULL, 0);
    if (buffer == (void *)-1) {
        perror("shmat failed");
        return 1;
    }

    sem_t *empty = sem_open("/sem_empty", 0);
    sem_t *full  = sem_open("/sem_full", 0);
    sem_t *mutex = sem_open("/sem_mutex", 0);

    if (empty == SEM_FAILED || full == SEM_FAILED || mutex == SEM_FAILED) {
        perror("sem_open failed");
        return 1;
    }

    for (int i = 0; i < num_items; i++) {

        sem_wait(full);
        sem_wait(mutex);

        item_t item = buffer->buffer[buffer->tail];

        buffer->tail = (buffer->tail + 1) % BUFFER_SIZE;
        buffer->count--;

        printf("Consumer %d: Consumed value %d from Producer %d\n",
               consumer_id, item.value, item.producer_id);

        sem_post(mutex);
        sem_post(empty);

        sleep(1);
    }

    shmdt(buffer);

    sem_close(empty);
    sem_close(full);
    sem_close(mutex);

    return 0;
}
