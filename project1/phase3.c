#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void initialize_accounts() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

void cleanup_mutexes() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }
}

void transfer_deadlock(int from, int to, double amount) {
    printf("Thread %lu: trying to lock Account %d\n", pthread_self(), from);
    pthread_mutex_lock(&accounts[from].lock);
    printf("Thread %lu: locked Account %d\n", pthread_self(), from);

    usleep(200000);
    printf("Thread %lu: trying to lock Account %d\n", pthread_self(), to);
    pthread_mutex_lock(&accounts[to].lock);
    printf("Thread %lu: locked Account %d\n", pthread_self(), to);

    if (accounts[from].balance >= amount) {
        accounts[from].balance -= amount;
        accounts[to].balance += amount;
        printf("Thread %lu: transferred $%.2f from %d to %d\n",
               pthread_self(), amount, from, to);
    }

    pthread_mutex_unlock(&accounts[to].lock);
    pthread_mutex_unlock(&accounts[from].lock);
}

void* thread_one(void* arg) {
    transfer_deadlock(0, 1, 100.0);
    return NULL;
}

void* thread_two(void* arg) {
    transfer_deadlock(1, 0, 200.0);
    return NULL;
}

int main() {
    pthread_t t1, t2;

    initialize_accounts();

    printf("Phase 3: Deadlock Creation Demo\n\n");
    printf("If the program hangs after both threads lock one account,\n");
    printf("that means deadlock happened.\n\n");

    pthread_create(&t1, NULL, thread_one, NULL);
    pthread_create(&t2, NULL, thread_two, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    printf("\nThis line will likely never print because of deadlock.\n");

    cleanup_mutexes();
    return 0;
}
