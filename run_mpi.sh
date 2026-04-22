#!/bin/bash

# =============================================================================
# run_mpi.sh: Strong and Weak Scaling for OpenMPI
# =============================================================================

# --- Configuration ---
DIM=2048                    # Fixed dimension for Strong Scaling
BASE_DIM_WEAK=1024          # Base dimension for Weak Scaling (1 thread)
THREADS=(2 4 8)             # Thread counts to test
RUNS=3                      # Number of runs to average (Seeds/Iterations)
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR
STRONG_OUTPUT="$RESULTS_DIR/strong_mpi.csv"
WEAK_OUTPUT="$RESULTS_DIR/weak_mpi.csv"

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

# Get T1 (Time for 2 threads) to calculate speedup
T1_STRONG=$(average_time "mpirun -n 2 ./target/main_openmpi $DIM")

for t in "${THREADS[@]}"; do
    avg_time=$(average_time "mpirun -n $t ./target/main_openmpi $DIM")
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
T1_WEAK=$(average_time "mpirun -n 2 ./target/main_openmpi $BASE_DIM_WEAK")

for t in "${THREADS[@]}"; do
    # For Matrix Multiplication, work is N^3. To keep work-per-thread constant:
    # New_N = Base_N * (threads ^ 1/3)
    current_dim=$(awk "BEGIN {printf \"%d\", $BASE_DIM_WEAK * ($t ^ (1.0/3.0))}")

    avg_time=$(average_time "mpirun -n $t ./target/main_openmpi $current_dim")
    scaled_speedup=$(awk "BEGIN {printf \"%.6f\", ($t * $T1_WEAK) / $avg_time}")

    echo "$current_dim,$t,$avg_time,$scaled_speedup" >> $WEAK_OUTPUT
    echo "  Dim: $current_dim | Threads: $t | Time: ${avg_time}s | Scaled Speedup: ${scaled_speedup}x"
done

echo -e "\nDone! Data saved to $STRONG_OUTPUT and $WEAK_OUTPUT"
