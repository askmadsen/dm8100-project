#include <cuda_runtime.h>
#include <stdio.h>

#include "matrix.h"

__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c) {
    int workIndex = threadIdx.x + blockDim.x * blockIdx.x;
    printf("???\n");

    if (workIndex < a.cols * a.rows) {
        int i = workIndex / a.cols;
        int j = workIndex % a.cols;
        float* c_ptr = c.ptr + i * c.cols + j;

        for (int k = 0; k < a.cols; k++) {
            float* a_ptr = a.ptr + i * a.cols + k;
            float* b_ptr = bT.ptr + j * bT.cols + k;
            *c_ptr += *a_ptr * *b_ptr;
        }

        printf("%d %d %d %f\n", workIndex, i, j, *c_ptr);
    }
}