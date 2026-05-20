#!/bin/bash

# =============================================================================
# run_openmp.sh: Strong and Weak Scaling for OpenMP
# =============================================================================

# --- Configuration ---
DIMS=(1024 2048 4096 8192)          # Matrix dimensions to test
THREADS=(12) # Thread counts to test
RUNS=3                      # Number of runs to average (Seeds/Iterations)
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR
OPENMP_OUTPUT="$RESULTS_DIR/openmp_results.csv"

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
# OPENMP EXECUTION
# =============================================================================
echo "Dim,Threads,Algorithm,Execution_Time" > $OPENMP_OUTPUT

# Get T1 (Time for 1 thread) to calculate speedup
for dim in "${DIMS[@]}"; do
    for threads in "${THREADS[@]}"; do
        for alg in "blocks"; do
            avg_time=$(average_time "./target/main_openmp $dim $threads --alg $alg")
            echo "$dim,$threads,$alg,$avg_time" >> $OPENMP_OUTPUT
            printf "  Dim: %-5s | Threads: %-5s | Algorithm: %-10s | Time: %10ss\n" \
                   "$dim" "$threads" "$alg" "$avg_time"
        done
    done
done
