#!/bin/bash
set -e

mkdir -p logs results
./bin/simulation_unsafe | tee logs/unsafe.log results/unsafe_results.txt
