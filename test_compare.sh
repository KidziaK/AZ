#!/bin/bash

N=${1:-10}  # Number of tests, default 10
GENERATOR=./build/test_case_generator/test_case_generator.exe
BRUTE=./build/pattern_matching_bruteforce/pattern_matching_bruteforce.exe
FAST=./build/pattern_matching_v2/app.exe

for i in $(seq 1 $N); do
    INPUT="test_input.txt"
    OUT_BRUTE="out_brute.txt"
    OUT_FAST="out_fast.txt"

    $GENERATOR 10 10 10000 10000 abcd $i $INPUT

    start_brute=$(date +%s%N)
    $BRUTE < $INPUT > $OUT_BRUTE
    end_brute=$(date +%s%N)
    time_brute=$(( (end_brute - start_brute)/1000000 ))

    start_fast=$(date +%s%N)
    $FAST $INPUT $OUT_FAST
    end_fast=$(date +%s%N)
    time_fast=$(( (end_fast - start_fast)/1000000 ))

    echo "Running test $i/$N... Brute: ${time_brute}ms, Fast: ${time_fast}ms"
    
    # Remove the first line (count) from both outputs for comparison, or compare as-is if you want
    if ! diff -q $OUT_BRUTE $OUT_FAST > /dev/null; then
        echo "Difference found in test $i"
        echo "Input:"
        cat $INPUT
        echo "Bruteforce output:"
        cat $OUT_BRUTE
        echo "Pattern_matching_v2 output:"
        cat $OUT_FAST
        exit 1
    fi

    # Optionally, clean up
    rm $INPUT $OUT_BRUTE $OUT_FAST

done

echo "All $N tests passed!"