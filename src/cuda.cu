#include <cuda_runtime.h>
#include "cuda.h"

__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c) {
    int workIndex = threadIdx.x + blockDim.x * blockIdx.x

    if (workIndex < a.cols * a.rows) {
        int i = workIndex / a.cols;
        int j = workIndex % a.cols;

        for (int k = 0; k < a.cols; k++) {
            *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, j, k);
        }
    }
}
