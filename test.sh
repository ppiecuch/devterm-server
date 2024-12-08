#!/bin/bash

./devterm_tool "{div=$((RANDOM % 10))}"
for f in $(seq 0 4); do
    ./devterm_tool "{font=$f}Test font index $f{nl}"
done
for f in $(seq 0 4); do
    ./devterm_tool "{font=${f}a}Test font index $f{nl}"
done
for f in $(seq 0 4); do
    ./devterm_tool "{font=${f}u}Test font index $f{nl}"
done
./devterm_tool "{div=$((RANDOM % 10))}"
./devterm_tool "{page}"
