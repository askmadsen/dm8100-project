#pragma once

#include "matrix.h"

__global__ void cuda_matmul_transposed(Matrix a, Matrix bT, Matrix c);
