#!/bin/bash
set -e

echo "============================================================"
echo " SprintBite OS - PHP GUI"
echo " Open in browser: http://localhost:8080"
echo " Make sure the TCP server is running first:"
echo " ./scripts/run_server.sh"
echo "============================================================"

php -S localhost:8080 -t gui
