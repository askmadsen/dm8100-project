#pragma once

#include "matrix.h"

// Performs matrix multiplication using a simple triple-nested loop, parallelized with OpenMP.
void matmul_openmp(Matrix a, Matrix b, Matrix c);

// Performs matrix multiplication using a transposed matrix to reduce cache misses, parallelized with OpenMP.
void matmul_openmp_transposed(Matrix a, Matrix b, Matrix c);

// Performs matrix multiplication by in a recursive divide-and-conquer manner,
// creating 8 subproblems, parallelized with OpenMP when the subproblems are within the threshold.
void matmul_openmp_recursive(Matrix a, Matrix b, Matrix c, int threshold);

// Performs matrix multiplication using shared-memory tiling (TILE x TILE blocks),
// loading tiles of A and B to reduce global memory traffic.
void matmul_openmp_blocks(Matrix a, Matrix b, Matrix c);
