#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_ACCOUNTS 2
#define NUM_THREADS 4
#define TRANSACTIONS_PER_THREAD 10000
#define INITIAL_BALANCE 1000.0

typedef struct {
    int account_id;
    double balance;
    int transaction_count;
} Account;

Account accounts[NUM_ACCOUNTS];

void deposit_unsafe(int account_id, double amount) {
    double current_balance = accounts[account_id].balance;
    usleep(1);
    double new_balance = current_balance + amount;
    accounts[account_id].balance = new_balance;
    accounts[account_id].transaction_count++;
}

void withdraw_unsafe(int account_id, double amount) {
    double current_balance = accounts[account_id].balance;
    usleep(1);
    double new_balance = current_balance - amount;
    accounts[account_id].balance = new_balance;
    accounts[account_id].transaction_count++;
}

void* thread_work(void* arg) {
    for (int i = 0; i < TRANSACTIONS_PER_THREAD; i++) {
        int account_id = rand() % NUM_ACCOUNTS;
        double amount = (rand() % 100) + 1;

        if (rand() % 2 == 0) {
            deposit_unsafe(account_id, amount);
        } else {
            withdraw_unsafe(account_id, amount);
        }
    }
    return NULL;
}

int main() {
    srand(time(NULL));

    pthread_t threads[NUM_THREADS];

    // Initializes accounts
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        accounts[i].account_id = i;
        accounts[i].balance = INITIAL_BALANCE;
        accounts[i].transaction_count = 0;
    }

    // Creates the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_work, NULL);
    }

    // Joins the threads
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    // Prints the final results
    printf("Final Account Balances:\n");
    for (int i = 0; i < NUM_ACCOUNTS; i++) {
        printf("Account %d: Balance = %.2f | Transactions = %d\n",
               accounts[i].account_id,
               accounts[i].balance,
               accounts[i].transaction_count);
    }

    return 0;
}
