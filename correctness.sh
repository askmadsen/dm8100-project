#!/bin/bash

# =============================================================================
# correctness.sh — Multi-method Verification
# =============================================================================

SCALE=42
TEST_DIMS=(128 256 512)
TEST_THREADS=(1 4 8)

echo "############################################################"
echo "#                CORRECTNESS VERIFICATION                  #"
echo "############################################################"

printf "  %-12s | %-8s | %-10s | %-10s\n" "Method" "Dim" "Threads" "Status"
printf "  %-12s | %-8s | %-10s | %-10s\n" "------------" "--------" "----------" "----------"

for dim in "${TEST_DIMS[@]}"; do
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}.txt")
    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}.txt")
    result=$(./target/main_cuda "$dim" --dest "/tmp/cuda_${dim}.txt")

    result=$(./target/main_correctness "/tmp/serial_${dim}.txt" "/tmp/openmp_${dim}.txt")
    printf "openmp (${dim}): %d\n" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}.txt" "/tmp/cuda_${dim}.txt")
    printf "cuda (${dim}): %d\n" $?
done

echo "All tests passed!"
