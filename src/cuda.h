#pragma once

#include "matrix.h"

// Grid-stride loop where each thread computes individual output elements.
// Expects B to be pre-transposed to allow sequential memory walks in the inner loop.
__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c);

// Grid-stride loop where each thread sequentially processes an entire 2D sub-tile (chunk).
// Uses an i->k->j loop order for A-cache reuse.
__global__ void matmul_cuda_chunks(Matrix a, Matrix b, Matrix c, int chunk_size);

// Performs matrix multiplication using shared-memory tiling (TILE x TILE blocks),
// loading tiles of A and B to reduce global memory traffic.
__global__ void matmul_cuda_blocks(Matrix a, Matrix b, Matrix c);
