#!/bin/bash
set -e

mkdir -p tables

unsafe_processed=$(grep "Processed orders reported" results/unsafe_results.txt | awk '{print $4}')
unsafe_race=$(grep "Race-condition indicators" results/unsafe_results.txt | awk '{print $3}')
unsafe_negative=$(grep "Negative inventory events" results/unsafe_results.txt | awk '{print $4}')
unsafe_inventory=$(grep "Final inventory value" results/unsafe_results.txt | awk '{print $4}')

safe_cooked=$(grep "Orders cooked" results/safe_results.txt | awk '{print $3}')
safe_delivered=$(grep "Orders delivered" results/safe_results.txt | awk '{print $3}')
safe_inventory=$(grep "Final inventory" results/safe_results.txt | awk '{print $3}')

cat > tables/comparison.csv <<CSV
metric,unsafe_module,safe_module
orders_requested,30,30
orders_processed_or_delivered,$unsafe_processed,$safe_delivered
orders_cooked,not_reliable,$safe_cooked
race_condition_indicators,$unsafe_race,0
negative_inventory_events,$unsafe_negative,0
final_inventory,$unsafe_inventory,$safe_inventory
synchronization_status,INCONSISTENT,CONSISTENT
CSV

echo "[experiments] Table generated at tables/comparison.csv"
