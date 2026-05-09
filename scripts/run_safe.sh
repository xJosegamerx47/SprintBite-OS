#!/bin/bash
set -e

mkdir -p logs results
./bin/simulation_safe | tee logs/safe.log results/safe_results.txt
