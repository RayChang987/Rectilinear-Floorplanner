#!/bin/bash
# Runs the floorplanner on all test cases; results go to out/<case>/check.txt.
# Usage: ./run_script.sh [seed]
set -e
BIN=./build/src/floorplanner
SEED=${1:-1}
for i in 01 02 03 04 05 06 07 08 09 10; do
    mkdir -p out/case$i
    $BIN -i testcase/case$i-input.txt -o out/case$i -s "$SEED" < /dev/null \
        > out/case$i/log.txt
done
