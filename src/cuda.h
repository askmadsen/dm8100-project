#pragma once

#include "matrix.h"

__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c);
