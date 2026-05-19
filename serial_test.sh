#!/bin/bash

# =============================================================================
# run_openmp.sh: Strong and Weak Scaling for OpenMP
# =============================================================================

# --- Configuration ---
DIMS=(8192)          # Matrix dimensions to test
RUNS=3                      # Number of runs to average (Seeds/Iterations)
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR
SERIAL_OUTPUT="$RESULTS_DIR/serial_results.csv"

# Helper: runs command RUNS times and returns average wall time
average_time() {
    local cmd="$*"
    local total=0
    for i in $(seq 1 $RUNS); do
        t=$(eval "$cmd")
        total=$(awk "BEGIN {print $total + $t}")
    done
    awk "BEGIN {printf \"%.6f\", $total / $RUNS}"
}

# =============================================================================
# SERIAL EXECUTION
# =============================================================================
echo "Dim,Algorithm,Execution_Time" > $SERIAL_OUTPUT

# Get T1 (Time for 1 thread) to calculate speedup
for dim in "${DIMS[@]}"; do
    for alg in "ikj" "chunks"; do
        avg_time=$(average_time "./target/main_serial $dim --alg $alg")
        echo "$dim,$alg,$avg_time" >> $SERIAL_OUTPUT
        printf "  Dim: %-5s | Algorithm: %-10s | Time: %10ss\n" \
               "$dim" "$alg" "$avg_time"
    done
done
