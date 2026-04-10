#!/bin/bash

# =============================================================================
# scaling.sh — Strong and weak scaling benchmark
# Runs each configuration RUNS times and averages the result
# Output: results/strong_scaling.csv and results/weak_scaling.csv
# =============================================================================

DIMS=(512 1024)         # Matrix dimensions for strong scaling
THREADS=(1 2 4 8)       # Thread counts to test
CORES=(2 4 8)           # Number of MPI processes
BASE_DIM=1000           # Base dimension for weak scaling (1 thread)
RUNS=3                  # Number of runs to average over
RESULTS_DIR="results"

mkdir -p $RESULTS_DIR
STRONG_OUTPUT="$RESULTS_DIR/strong_scaling.csv"
WEAK_OUTPUT="$RESULTS_DIR/weak_scaling.csv"

# Helper: run a command RUNS times and return the average wall time
# Usage: average_time ./target/main_serial 512 42
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
# Fixed problem size, varying thread count
# Speedup = serial_time / parallel_time
# =============================================================================

echo "method,dim,threads,time_avg,speedup" > $STRONG_OUTPUT

echo ""
echo "############################################################"
echo "#                    STRONG SCALING                        #"
echo "############################################################"

echo ""
echo "--- Serial baseline ---"
printf "  %-8s | %-12s\n" "dim" "time (s)"
printf "  %-8s | %-12s\n" "--------" "------------"
for dim in "${DIMS[@]}"; do
    avg=$(average_time "./target/main_serial $dim")
    echo "serial,$dim,1,$avg,1.000000" >> $STRONG_OUTPUT
    printf "  %-8s | %-12s\n" "$dim" "${avg}s"
done

echo ""
echo "--- OpenMP ---"
printf "  %-8s | %-10s | %-12s | %-10s\n" "dim" "threads" "time (s)" "speedup"
printf "  %-8s | %-10s | %-12s | %-10s\n" "--------" "----------" "------------" "----------"
for dim in "${DIMS[@]}"; do
    serial_time=$(grep "^serial,$dim," $STRONG_OUTPUT | cut -d',' -f4)
    for threads in "${THREADS[@]}"; do
        avg=$(average_time "./target/main_openmp $dim $threads")
        speedup=$(awk "BEGIN {printf \"%.6f\", $serial_time / $avg}")
        echo "openmp,$dim,$threads,$avg,$speedup" >> $STRONG_OUTPUT
        printf "  %-8s | %-10s | %-12s | %-10s\n" "$dim" "$threads" "${avg}s" "${speedup}x"
    done
    echo ""
done

echo "Strong scaling results saved to $STRONG_OUTPUT"

# =============================================================================
# WEAK SCALING
# Problem size scales with threads to keep work-per-thread constant
# Since matmul scales as N^3: N = BASE_DIM * threads^(1/3)
# Ideal result: time stays flat as threads and N grow together
# =============================================================================

echo "method,dim,threads,time_avg" > $WEAK_OUTPUT

echo ""
echo "############################################################"
echo "#                     WEAK SCALING                        #"
echo "############################################################"
echo ""
echo "--- OpenMP (base dim=$BASE_DIM, scaling N = base * threads^(1/3)) ---"
printf "  %-8s | %-10s | %-12s\n" "dim" "threads" "time (s)"
printf "  %-8s | %-10s | %-12s\n" "--------" "----------" "------------"
for threads in "${THREADS[@]}"; do
    dim=$(awk "BEGIN {printf \"%d\", $BASE_DIM * ($threads ^ (1.0/3.0))}")
    avg=$(average_time "./target/main_openmp $dim $threads")
    echo "openmp_weak,$dim,$threads,$avg" >> $WEAK_OUTPUT
    printf "  %-8s | %-10s | %-12s\n" "$dim" "$threads" "${avg}s"
done

echo ""
echo "Weak scaling results saved to $WEAK_OUTPUT"
echo ""
echo "Done!"
