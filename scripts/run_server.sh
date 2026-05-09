#!/bin/bash
set -e

mkdir -p logs results
./bin/server | tee logs/server.log results/server_results.txt
