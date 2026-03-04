#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSACTIONS_PER_THREAD 10
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void initialize_accounts() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }
}

void cleanup_mutexes() {
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }
}

void transfer_safe(int from, int to, double amount) {

    int first = from < to ? from : to;
    int second = from < to ? to : from;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    if (accounts[from].balance >= amount) {

        accounts[from].balance -= amount;
        accounts[to].balance += amount;

        accounts[from].transaction_count++;
        accounts[to].transaction_count++;

        printf("Teller %lu: Transferred $%.2f from Account %d to Account %d\n",
               pthread_self(), amount, from, to);
    }

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

void* teller_thread(void* arg) {

    int id = *(int*)arg;

    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {

        int from = rand() % NUM_ACCOUNTS;
        int to = rand() % NUM_ACCOUNTS;

        while (to == from) {
            to = rand() % NUM_ACCOUNTS;
        }

        double amount = (rand() % 100) + 1;

        transfer_safe(from, to, amount);
    }

    return NULL;
}

int main() {

    pthread_t threads[NUM_THREADS];
    int ids[NUM_THREADS];

    srand(time(NULL));

    printf("Phase 3: Deadlock Prevention Demo \n\n");

    initialize_accounts();

    printf("Initial State:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%.2f\n", i, accounts[i].balance);
    }

    printf("\nExpected total : $%.2f\n\n", NUM_ACCOUNTS * INITIAL_BALANCE);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (int i = 0; i < NUM_THREADS; i++) {
        ids[i] = i;
        pthread_create(&threads[i], NULL, teller_thread, &ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    printf("\nFinal Result\n");

    double total = 0;

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: $%.2f (%d transactions)\n",
               i, accounts[i].balance, accounts[i].transaction_count);
        total += accounts[i].balance;
    }

    double expected = NUM_ACCOUNTS * INITIAL_BALANCE;

    printf("\nExpected total : $%.2f\n", expected);
    printf("Actual total   : $%.2f\n", total);
    printf("Difference     : $%.2f\n", total - expected);

    if (total == expected) {
        printf("\nWorking!\n");
    }

    double elapsed =
        (end.tv_sec - start.tv_sec) +
        (end.tv_nsec - start.tv_nsec) / 1000000000.0;

    printf("\nExecution time : %.4f seconds\n", elapsed);

    cleanup_mutexes();

    return 0;
}
