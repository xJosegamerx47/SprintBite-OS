#!/bin/bash
set -e

echo "============================================================"
echo " SprintBite OS - Live client request simulation"
echo " Sending requests to TCP server in real time"
echo "============================================================"

orders=(
    "Pizza pepperoni"
    "Hamburguesa doble"
    "Tacos al pastor"
    "Sushi roll"
    "Orden premium: ramen"
    "Orden urgente: burrito"
    "Combo familiar"
    "Ensalada vegetariana"
)

for order in "${orders[@]}"; do
    echo "[CLIENT SCRIPT] Sending order: $order"
    ./scripts/run_client.sh "$order"
    sleep 1
done

echo "[CLIENT SCRIPT] Sending shutdown signal..."
./scripts/run_client.sh "exit"

echo "============================================================"
echo " Live client request simulation finished"
echo "============================================================"
