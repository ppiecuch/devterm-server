#!/bin/bash

./devterm_tool "{div=$((RANDOM % 10)),time}"
./devterm_tool "{nl}[[ TEST 1 ]]{nl}{nl}"
for f in $(seq 0 4); do
    ./devterm_tool "{font=$f}Test font index $f{nl}"
done
./devterm_tool "{nl}[[ TEST 2 ]]{nl}{nl}"
for f in $(seq 0 4); do
    ./devterm_tool "{font=${f}a}Test font index $f{nl}"
done
./devterm_tool "{nl}[[ TEST 3 ]]{nl}{nl}"
for f in $(seq 0 4); do
    ./devterm_tool "{font=${f}u}Test font index $f{nl}"
done
./devterm_tool "{div=$((RANDOM % 10))}"
./devterm_tool "{page}"
./devterm_tool "{selftest}"
./devterm_tool "{page}"
