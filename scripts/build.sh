#!/bin/bash
set -e

mkdir -p bin logs results tables

echo "[build] Compiling SprintBite OS modules..."
gcc -O2 -Wall -Wextra -pthread src/simulation_unsafe.c -o bin/simulation_unsafe
gcc -O2 -Wall -Wextra -pthread src/simulation_safe.c -o bin/simulation_safe
echo "[build] Done. Binaries are in ./bin"
