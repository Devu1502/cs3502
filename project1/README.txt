Project 1 – Multi-Threaded Banking System
CS 3502 – Operating Systems
Professor Regan

Overview:
This project demonstrates multi-threaded programming concepts using a simulated banking system. The program creates multiple threads that perform deposits and withdrawals on shared bank accounts.

Phases:

Phase 1 – Race Condition
Implements concurrent transactions without synchronization to demonstrate race conditions and inconsistent balances.

Phase 2 – Mutex Protection
Introduces mutex locks to protect shared account data and eliminate race conditions.

Phase 3 – Deadlock
Creates a scenario where multiple locks can cause a deadlock between threads.

Phase 4 – Deadlock Resolution
Resolves the deadlock using proper lock ordering or synchronization strategy.

Compilation:
gcc -Wall -Wextra -pthread phaseX.c -o phaseX

Execution:
./phaseX
