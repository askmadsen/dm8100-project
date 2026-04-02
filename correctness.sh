#!/bin/bash

# =============================================================================
# correctness.sh — Multi-method Verification
# =============================================================================

SCALE=42
TEST_DIMS=(512 873 1024)
TEST_THREADS=(1 4 8)
METHODS=("openmp") # Add "cuda" "mpi" here they are implemented

echo "############################################################"
echo "#                CORRECTNESS VERIFICATION                  #"
echo "############################################################"

printf "  %-12s | %-8s | %-10s | %-10s\n" "Method" "Dim" "Threads" "Status"
printf "  %-12s | %-8s | %-10s | %-10s\n" "------------" "--------" "----------" "----------"

for method in "${METHODS[@]}"; do
    for dim in "${TEST_DIMS[@]}"; do
        for threads in "${TEST_THREADS[@]}"; do

            # Note the extra $method argument here
            result=$(./target/main_correctness "$method" "$dim" "$SCALE" "$threads")
            exit_code=$?

            status="[OK]"
            if [ $exit_code -ne 0 ]; then status="[FAILED]"; fi

            printf "  %-12s | %-8s | %-10s | %-10s\n" "$method" "$dim" "$threads" "$status"

            if [ $exit_code -ne 0 ]; then
                echo -e "\nCRITICAL ERROR in $method\n$result"
                exit 1
            fi
        done
    done
    echo "------------------------------------------------------------"
done

echo "All tests passed!"
