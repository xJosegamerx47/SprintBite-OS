# SprintBite OS

SprintBite OS is a Linux-based Operating Systems project that simulates a concurrent food-order processing system inspired by delivery platforms.

The project demonstrates concurrency problems and synchronization solutions using C, pthreads, mutexes, semaphores, and producer-consumer queues.

## Workflow

Clients -> Order Queue -> Cooks -> Packing Area -> Ready Orders -> Couriers

## Modules

### Unsafe module

`src/simulation_unsafe.c`

This module intentionally avoids synchronization. It demonstrates race conditions, inconsistent access to shared counters, unsafe inventory updates, and unreliable execution behavior.

### Safe module

`src/simulation_safe.c`

This module uses pthreads, mutexes, semaphores, producer-consumer queues, limited stoves, and limited packing slots.

## Dependencies

```bash
sudo apt update
sudo apt install build-essential make gcc -y

Build
chmod +x scripts/*.sh
./scripts/build.sh
Run experiments
./scripts/run_unsafe.sh
./scripts/run_safe.sh
./scripts/run_experiments.sh
cat tables/comparison.csv
Evidence generated
logs/unsafe.log
logs/safe.log
results/unsafe_results.txt
results/safe_results.txt
tables/comparison.csv
Observed results

The unsafe module shows race-condition indicators and an inconsistent synchronization status.

The safe module generates, cooks, and delivers all 30 orders correctly. It finishes with a consistent synchronization status.

Operating Systems concepts demonstrated
Threads
Concurrency
Shared resources
Critical sections
Race conditions
Mutex synchronization
Semaphores
Producer-consumer queues
Limited resource management
Reproducible experiments
