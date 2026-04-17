#!/bin/bash

# =============================================================================
# run_openmp.sh: Strong and Weak Scaling for OpenMP
# =============================================================================

# --- Configuration ---
DIM=2048                    # Fixed dimension for Strong Scaling
BASE_DIM_WEAK=1024          # Base dimension for Weak Scaling (1 thread)
THREADS=(1 2 4 8 12 16 20 28) # Thread counts to test
RUNS=3                      # Number of runs to average (Seeds/Iterations)
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR
STRONG_OUTPUT="$RESULTS_DIR/strong_openmp.csv"
WEAK_OUTPUT="$RESULTS_DIR/weak_openmp.csv"

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
# STRONG SCALING
# Fixed Dim, Varying Threads
# =============================================================================
echo "Dim,Threads,Execution_Time,Speedup" > $STRONG_OUTPUT

echo "Running Strong Scaling (Fixed Dim: $DIM)..."

# Get T1 (Time for 1 thread) to calculate speedup
T1_STRONG=$(average_time "./target/main_openmp $DIM 1")

for t in "${THREADS[@]}"; do
    avg_time=$(average_time "./target/main_openmp $DIM $t")
    speedup=$(awk "BEGIN {printf \"%.6f\", $T1_STRONG / $avg_time}")

    echo "$DIM,$t,$avg_time,$speedup" >> $STRONG_OUTPUT
    echo "  Threads: $t | Time: ${avg_time}s | Speedup: ${speedup}x"
done

# =============================================================================
# WEAK SCALING
# Work scales with threads (N = BASE * threads^(1/3) for MatMul)
# =============================================================================
echo "Dim,Threads,Execution_Time,Scaled_Speedup" > $WEAK_OUTPUT

echo -e "\nRunning Weak Scaling (Work grows with Threads)..."

# Get T1 for weak scaling baseline
T1_WEAK=$(average_time "./target/main_openmp $BASE_DIM_WEAK 1")

for t in "${THREADS[@]}"; do
    # For Matrix Multiplication, work is N^3. To keep work-per-thread constant:
    # New_N = Base_N * (threads ^ 1/3)
    current_dim=$(awk "BEGIN {printf \"%d\", $BASE_DIM_WEAK * ($t ^ (1.0/3.0))}")

    avg_time=$(average_time "./target/main_openmp $current_dim $t")
    scaled_speedup=$(awk "BEGIN {printf \"%.6f\", ($t * $T1_WEAK) / $avg_time}")

    echo "$current_dim,$t,$avg_time,$scaled_speedup" >> $WEAK_OUTPUT
    echo "  Dim: $current_dim | Threads: $t | Time: ${avg_time}s | Scaled Speedup: ${scaled_speedup}x"
done

echo -e "\nDone! Data saved to $STRONG_OUTPUT and $WEAK_OUTPUT"
