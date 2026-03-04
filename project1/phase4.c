#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define NUM_TRANSFERS 10
#define INITIAL_BALANCE 1000.0

typedef struct {
    int id;
    double balance;
    int transaction_count;
    pthread_mutex_t lock;
} Account;

Account accounts[NUM_ACCOUNTS];

void safe_transfer_ordered(int teller_id, int from, int to, double amount) {
    int first  = (from < to) ? from : to;
    int second = (from < to) ? to   : from;

    pthread_mutex_lock(&accounts[first].lock);
    pthread_mutex_lock(&accounts[second].lock);

    if (accounts[from].balance >= amount) {
        accounts[from].balance -= amount;
        accounts[to].balance   += amount;
        accounts[from].transaction_count++;
        accounts[to].transaction_count++;
        printf("Teller %d: Transferred $%.2f from Account %d to Account %d\n",
               teller_id, amount, from, to);
    }

    pthread_mutex_unlock(&accounts[second].lock);
    pthread_mutex_unlock(&accounts[first].lock);
}

void *teller(void *arg) {
    int teller_id = *(int *)arg;
    unsigned int seed = (unsigned int)(time(NULL) ^ pthread_self());

    for (int i = 0; i < NUM_TRANSFERS; i++) {
        int from      = rand_r(&seed) % NUM_ACCOUNTS;
        int to        = (from + 1) % NUM_ACCOUNTS;
        double amount = (double)(rand_r(&seed) % 100) + 1.0;
        safe_transfer_ordered(teller_id, from, to, amount);
        usleep(1000);
    }
    return NULL;
}

int main() {
    printf("Phase 4: Deadlock Resolution (Lock Ordering)\n\n");

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
        pthread_mutex_init(&accounts[i].lock, NULL);
    }

    printf("Initial State:\n");
    double expected_total = 0.0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("  Account %d: $%.2f\n", i, accounts[i].balance);
        expected_total += accounts[i].balance;
    }
    printf("\nExpected total: $%.2f\n\n", expected_total);

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        pthread_create(&threads[i], NULL, teller, &thread_ids[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = (t_end.tv_sec  - t_start.tv_sec) +
                     (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    printf("\nFinal Results:\n");
    double actual_total = 0.0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("  Account %d: $%.2f (%d transactions)\n",
               i, accounts[i].balance, accounts[i].transaction_count);
        actual_total += accounts[i].balance;
    }

    printf("\nExpected total : $%.2f\n", expected_total);
    printf("Actual total   : $%.2f\n", actual_total);
    printf("Difference     : $%.2f\n", actual_total - expected_total);

    if (fabs(actual_total - expected_total) < 0.01)
        printf("\nNo deadlock detected.\n");
    else
        printf("\nDiscrepancy found\n");

    printf("\nExecution time : %.4f seconds\n", elapsed);

    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        pthread_mutex_destroy(&accounts[i].lock);
    }

    return 0;
}
