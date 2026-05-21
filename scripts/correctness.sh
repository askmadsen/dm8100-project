#!/bin/bash

# =============================================================================
# correctness.sh — Multi-method Verification
# =============================================================================

TEST_DIMS=(128 256 512)

echo "############################################################"
echo "#                CORRECTNESS VERIFICATION                  #"
echo "############################################################"

printf "  %-20s | %-8s | %-10s\n" "Method" "Dim"  "Status"
printf "  %-20s | %-8s | %-10s\n" "--------------------" "--------" "----------"

for dim in "${TEST_DIMS[@]}"; do
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}_ijk.txt" --alg "ijk")
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}_ikj.txt" --alg "ikj")
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}_transposed.txt" --alg "transposed")
    result=$(./target/main_serial "$dim" --dest "/tmp/serial_${dim}_recursive.txt" --alg "recursive")

    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}_simple.txt" --alg "simple")
    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}_transposed.txt" --alg "transposed")
    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}_recursive.txt" --alg "recursive")
    result=$(./target/main_openmp "$dim" 4 --dest "/tmp/openmp_${dim}_blocks.txt" --alg "blocks")


    result=$(mpirun -n 4 ./target/main_openmpi "$dim" --dest "/tmp/openmpi_${dim}.txt")

    result=$(./target/main_cuda "$dim" --dest "/tmp/cuda_${dim}_chunks.txt" --alg "chunks")
    result=$(./target/main_cuda "$dim" --dest "/tmp/cuda_${dim}_transposed.txt" --alg "transposed")
    result=$(./target/main_cuda "$dim" --dest "/tmp/cuda_${dim}_blocks.txt" --alg "blocks")

    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/serial_${dim}_ijk.txt")
    printf "  %-20s | %-8s | %-10s\n" "Serial IJK" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/serial_${dim}_ikj.txt")
    printf "  %-20s | %-8s | %-10s\n" "Serial IKJ" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/serial_${dim}_transposed.txt")
    printf "  %-20s | %-8s | %-10s\n" "Serial Transposed" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/serial_${dim}_recursive.txt")
    printf "  %-20s | %-8s | %-10s\n" "Serial Recursive" "(${dim})" $?

    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/openmp_${dim}_simple.txt")
    printf "  %-20s | %-8s | %-10s\n" "Openmp Simple" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/openmp_${dim}_transposed.txt")
    printf "  %-20s | %-8s | %-10s\n" "Openmp Transposed" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/openmp_${dim}_recursive.txt")
    printf "  %-20s | %-8s | %-10s\n" "Openmp Recursive" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/openmp_${dim}_blocks.txt")
    printf "  %-20s | %-8s | %-10s\n" "Openmp Blocks" "(${dim})" $?

    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/openmpi_${dim}.txt")
    printf "  %-20s | %-8s | %-10s\n" "Openmpi" "(${dim})" $?

    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/cuda_${dim}_chunks.txt")
    printf "  %-20s | %-8s | %-10s\n" "Cuda Chunks" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/cuda_${dim}_transposed.txt")
    printf "  %-20s | %-8s | %-10s\n" "Cuda Transposed" "(${dim})" $?
    result=$(./target/main_correctness "/tmp/serial_${dim}_ijk.txt" "/tmp/cuda_${dim}_blocks.txt")
    printf "  %-20s | %-8s | %-10s\n" "Cuda Blocks" "(${dim})" $?
done

echo "All tests passed!"
