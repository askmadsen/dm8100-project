#!/bin/bash

# --- Configuration ---
DIM_STRONG=2048                 # Fixed dimension for Strong Scaling
BASE_DIM_WEAK=2048              # Base dimension for Weak Scaling (1 block)
FIXED_THREADS=256               # Fixed GPU Thread number
BLOCK_ARRAY=(1 2 4 8 12 16 20)     # Block counts to test
RUNS=3
RESULTS_DIR="results"


mkdir -p $RESULTS_DIR
STRONG_OUTPUT="$RESULTS_DIR/strong_cuda.csv"
WEAK_OUTPUT="$RESULTS_DIR/weak_cuda.csv"

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

# Helper: rounds the matrix dimension to nearest multiple of 128 so weak scalin produces meaningfull results
round_to_128() {
    local n=$1
    echo $(( (n + 64) / 128 * 128 ))
}

# =============================================================================
# STRONG SCALING
# =============================================================================
echo "Dim,Blocks,Threads,Execution_Time,Speedup" > $STRONG_OUTPUT
echo "Starting CUDA Strong Scaling (Fixed Dim: $DIM_STRONG)..."

# T1 Baseline: 1 Block
T1_STRONG=$(average_time "./target/main_cuda $DIM_STRONG --threads $FIXED_THREADS  --blocks 1 --alg chunks")

for blocks in "${BLOCK_ARRAY[@]}"; do
    avg_time=$(average_time "./target/main_cuda $DIM_STRONG --threads $FIXED_THREADS --blocks $blocks --alg chunks")
    speedup=$(awk "BEGIN {printf \"%.6f\", $T1_STRONG / $avg_time}")

    echo "$DIM_STRONG,$blocks,$FIXED_THREADS,$avg_time,$speedup" >> $STRONG_OUTPUT
    printf "  Blocks: %-3s | Threads: %-6s | Time: %10ss | Speedup: %sx\n" \
           "$blocks" "$FIXED_THREADS" "$avg_time" "$speedup"
done

# =============================================================================
# WEAK SCALING (Gustafson's Law)
# =============================================================================
echo "Dim,Blocks,Threads,Execution_Time,Scaled_Speedup" > $WEAK_OUTPUT
echo -e "\nStarting CUDA Weak Scaling..."

# Baseline for Weak Scaling (1 Block)
T1_WEAK=$(average_time "./target/main_cuda $BASE_DIM_WEAK --threads $FIXED_THREADS --blocks 1 --alg chunks")

for blocks in "${BLOCK_ARRAY[@]}"; do

    # Increase Dim to keep work-per-block constant
    raw_dim=$(awk "BEGIN {printf \"%d\", $BASE_DIM_WEAK * ($blocks ^ (1.0/3.0))}")

    # Round dim to nearest multiple of 128
    current_dim=$(round_to_128 $raw_dim)

    avg_time=$(average_time "./target/main_cuda $current_dim --threads $FIXED_THREADS --blocks $blocks --alg chunks")

    # Scaled Speedup calculation
    scaled_speedup=$(awk "BEGIN {printf \"%.6f\", ($blocks * $T1_WEAK) / $avg_time}")

    echo "$current_dim,$blocks,$FIXED_THREADS,$avg_time,$scaled_speedup" >> $WEAK_OUTPUT
    printf "  Dim: %-5s | Blocks: %-3s | Time: %10ss | Scaled Speedup: %sx\n" \
           "$current_dim" "$blocks" "$avg_time" "$scaled_speedup"
done
