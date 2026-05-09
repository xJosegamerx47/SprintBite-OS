# SprintBite OS

SprintBite OS is a Linux-based Operating Systems project that simulates a concurrent food-order processing system inspired by delivery platforms.

The project demonstrates concurrency problems and synchronization solutions using C, pthreads, mutexes, semaphores, producer-consumer queues, and TCP sockets.

## General idea

The system models a simplified delivery workflow:

```text
Clients -> Order Queue -> Cooks -> Packing Area -> Ready Orders -> Couriers

The project was also extended with a terminal-based client-server model:

TCP Client -> TCP Socket -> C Server
Main modules
1. Unsafe concurrency module

File:

src/simulation_unsafe.c

This module intentionally avoids synchronization. It demonstrates:

race conditions
inconsistent access to shared counters
unsafe inventory updates
unreliable execution behavior

Run with:

./scripts/run_unsafe.sh
2. Safe concurrency module

File:

src/simulation_safe.c

This module uses synchronization mechanisms:

pthread threads
mutexes
POSIX semaphores
producer-consumer queues
limited stoves
limited packing slots

Run with:

./scripts/run_safe.sh
3. TCP client-server module

Files:

src/server.c
src/client.c

This module implements a TCP server in C that receives food orders from terminal clients.

The server:

listens on port 5000
receives client messages through TCP sockets
registers each order
prints received messages in real time through stdout
handles each client connection with a separate thread
protects shared server statistics with a mutex

The client:

connects to the server through 127.0.0.1:5000
sends an order message
receives confirmation from the server

Run the server:

./scripts/run_server.sh

Send one client request:

./scripts/run_client.sh "Pizza pepperoni"

Stop the server:

./scripts/run_client.sh "exit"
4. Live client request script

File:

scripts/run_live_clients.sh

This script sends multiple orders automatically with pauses between each request. It is useful for demonstrating real-time client-server communication from the terminal.

Run with:

./scripts/run_live_clients.sh
Project structure
SprintBite-OS/
├── bin/
├── logs/
├── results/
├── screenshots/
├── scripts/
│   ├── build.sh
│   ├── run_unsafe.sh
│   ├── run_safe.sh
│   ├── run_experiments.sh
│   ├── run_server.sh
│   ├── run_client.sh
│   └── run_live_clients.sh
├── src/
│   ├── simulation_unsafe.c
│   ├── simulation_safe.c
│   ├── server.c
│   └── client.c
├── tables/
└── README.md
Dependencies

Install the required packages in Linux or WSL:

sudo apt update
sudo apt install build-essential make gcc -y

If you need to check occupied ports:

sudo apt install lsof -y
Build

Compile all C modules:

chmod +x scripts/*.sh
./scripts/build.sh

The compiled binaries are generated in:

bin/
Basic execution

Run the unsafe module:

./scripts/run_unsafe.sh

Run the safe module:

./scripts/run_safe.sh

Generate the comparison table:

./scripts/run_experiments.sh
cat tables/comparison.csv
Client-server execution

Use two WSL terminals.

Terminal 1:

cd /mnt/c/Users/Legion-Pro7/Documents/SprintBite-OS
./scripts/run_server.sh

Terminal 2:

cd /mnt/c/Users/Legion-Pro7/Documents/SprintBite-OS
./scripts/run_client.sh "Pizza pepperoni"
./scripts/run_client.sh "Hamburguesa doble"
./scripts/run_client.sh "Tacos al pastor"
./scripts/run_client.sh "exit"
Live client request demo

Terminal 1:

cd /mnt/c/Users/Legion-Pro7/Documents/SprintBite-OS
./scripts/run_server.sh

Terminal 2:

cd /mnt/c/Users/Legion-Pro7/Documents/SprintBite-OS
./scripts/run_live_clients.sh

This sends several client requests automatically so the server output can be observed in real time.

How to stop services

Stop the TCP server normally:

./scripts/run_client.sh "exit"

Or kill the process using port 5000:

sudo lsof -ti:5000 | xargs kill -9
Evidence generated

The project generates reproducible evidence in:

logs/
results/
tables/
screenshots/

Important evidence files include:

logs/unsafe.log
logs/safe.log
logs/server.log
results/unsafe_results.txt
results/safe_results.txt
results/server_results.txt
tables/comparison.csv
screenshots/
Operating Systems concepts demonstrated
threads
concurrency
race conditions
shared resources
critical sections
mutex synchronization
POSIX semaphores
producer-consumer queues
TCP sockets
client-server communication
one thread per client
inter-process communication
reproducible experiments
Expected results

The unsafe module should show race-condition indicators and an inconsistent synchronization status.

The safe module should generate, cook, and deliver all orders correctly with a consistent synchronization status.

The TCP client-server module should show the server receiving and registering client orders in real time.

Repository
https://github.com/xJosegamerx47/SprintBite-OS