#!/bin/bash

# =============================================================================
# correctness.sh — Multi-method Verification
# =============================================================================

TEST_DIMS=(128 256 512)

echo "############################################################"
echo "#                CORRECTNESS VERIFICATION                  #"
echo "############################################################"

printf "  %-12s | %-8s | %-10s\n" "Method" "Dim"  "Status"
printf "  %-12s | %-8s | %-10s\n" "------------" "--------" "----------"

for dim in "${TEST_DIMS[@]}"; do
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}.txt")
    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}.txt")
    result=$(mpirun -n 4 ./target/main_openmpi "$dim" --dest "/tmp/openmpi_${dim}.txt")
    result=$(./target/main_cuda "$dim" --dest "/tmp/cuda_${dim}.txt")

    result=$(./target/main_correctness "/tmp/serial_${dim}.txt" "/tmp/openmp_${dim}.txt")
    printf "  %-12s | %-8s | %-10s\n" "Openmp" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}.txt" "/tmp/openmpi_${dim}.txt")
    printf "  %-12s | %-8s | %-10s\n" "Openmpi" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}.txt" "/tmp/cuda_${dim}.txt")
    printf "  %-12s | %-8s | %-10s\n" "Cuda" "(${dim})" $?
done

echo "All tests passed!"
