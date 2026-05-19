#pragma once

#include "matrix.h"

__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c);

__global__ void matmul_cuda_chunks(Matrix a, Matrix b, Matrix c, int chunk_size);

__global__ void matmul_cuda_chunks2(Matrix a, Matrix b, Matrix c);