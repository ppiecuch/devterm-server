#!/bin/bash

./devterm_tool "{div=$((RANDOM % 10))}"
for f in $(seq 0 4); do
    ./devterm_tool "{font=$f}Test font index $f{nl}"
done
./devterm_tool "{page}"
