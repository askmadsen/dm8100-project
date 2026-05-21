#pragma once

#include "matrix.h"

// Performs matrix multiplication using a simple triple-nested loop, with the k-loop in the middle to improve cache locality.
void matmul_ikj(Matrix a, Matrix b, Matrix c);

// Performs matrix multiplication using a naive ijk loop order.
void matmul_ijk(Matrix a, Matrix b, Matrix c);

// Performs matrix multiplication using a transposed matrix to reduce cache misses.
void matmul_transposed(Matrix a, Matrix bT, Matrix c);

// Performs matrix multiplication in a recursive divide-and-conquer manner,
// each recursion creating 8 subproblems until the subproblems fall within the threshold,
// where the base case uses the simple matmul_ikj algorithm.
void matmul_recursive(Matrix a, Matrix b, Matrix c, int threshold);
