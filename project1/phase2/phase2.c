#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <string.h>
#include <math.h>

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

void transfer_safe(int from_id, int to_id, double amount) {
    pthread_mutex_lock(&accounts[from_id].lock);
    pthread_mutex_lock(&accounts[to_id].lock);

    if (accounts[from_id].balance >= amount) {
        accounts[from_id].balance -= amount;
        accounts[to_id].balance += amount;
        accounts[from_id].transaction_count++;
        accounts[to_id].transaction_count++;
    }

    pthread_mutex_unlock(&accounts[to_id].lock);
    pthread_mutex_unlock(&accounts[from_id].lock);
}

void *teller_thread(void *arg) {
    int teller_id = *(int *)arg;
    unsigned int seed = (unsigned int)(time(NULL) ^ pthread_self());

    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        int from = rand_r(&seed) % NUM_ACCOUNTS;
        int to = (from + 1) % NUM_ACCOUNTS;
        double amount = (double)(rand_r(&seed) % 100) + 1.0;

        transfer_safe(from, to, amount);
        printf("Teller %d: Transferred $%.2f from Account %d to Account %d\n",
               teller_id, amount, from, to);
    }
    return NULL;
}

int main() {
    printf("Phase 2: Mutex Protection Demo\n\n");

    initialize_accounts();

    printf("Initial State:\n");
    double expected_total = 0.0;
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("  Account %d: $%.2f\n", i, accounts[i].balance);
        expected_total += accounts[i].balance;
    }
    printf("\nExpected total (constant): $%.2f\n\n", expected_total);

    struct timespec t_start, t_end;
    clock_gettime(CLOCK_MONOTONIC, &t_start);

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;
        int rc = pthread_create(&threads[i], NULL, teller_thread, &thread_ids[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create failed: %s\n", strerror(rc));
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    clock_gettime(CLOCK_MONOTONIC, &t_end);
    double elapsed = (t_end.tv_sec - t_start.tv_sec) +
                     (t_end.tv_nsec - t_start.tv_nsec) / 1e9;

    printf("\nFinal Results \n");
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
        printf("\nWorking Correctly!\n");
    else
        printf("\nError\n");

    printf("\nExecution time : %.4f seconds\n", elapsed);

    cleanup_mutexes();
    return 0;
}

