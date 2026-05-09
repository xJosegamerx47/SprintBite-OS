#!/bin/bash
set -e

if [ -z "$1" ]; then
    echo "Usage: ./scripts/run_client.sh \"order description\""
    echo "Example: ./scripts/run_client.sh \"Pizza pepperoni\""
    exit 1
fi

./bin/client "$1"
