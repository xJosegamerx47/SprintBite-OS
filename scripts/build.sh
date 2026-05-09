#!/bin/bash
set -e

mkdir -p bin logs results tables

echo "[build] Compiling SprintBite OS modules..."
gcc -O2 -Wall -Wextra -pthread src/simulation_unsafe.c -o bin/simulation_unsafe
gcc -O2 -Wall -Wextra -pthread src/simulation_safe.c -o bin/simulation_safe
gcc -O2 -Wall -Wextra -pthread src/server.c -o bin/server
gcc -O2 -Wall -Wextra src/client.c -o bin/client
echo "[build] Done. Binaries are in ./bin"
