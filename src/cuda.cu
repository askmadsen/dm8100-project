#include <cuda_runtime.h>

#include "matrix.h"

/*__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c) {
    int workIndex = threadIdx.x + blockDim.x * blockIdx.x;

    if (workIndex >= a.cols * a.rows) {
        return;
    }

    int i = workIndex / a.cols;
    int j = workIndex % a.cols;
    float* c_ptr = c.ptr + i * c.cols + j;

    for (int k = 0; k < a.cols; k++) {
        float* a_ptr = a.ptr + i * a.cols + k;
        float* b_ptr = bT.ptr + j * bT.cols + k;
        *c_ptr += *a_ptr * *b_ptr;
    }
}*/

__global__ void matmul_cuda_transposed(Matrix a, Matrix bT, Matrix c) {
    int total_threads = blockDim.x * gridDim.x;

    for (int workIndex = threadIdx.x + blockDim.x * blockIdx.x;
         workIndex < a.rows * a.cols;
         workIndex += total_threads)
    {
        // --- Same logic as before, now inside the loop ---
        int i = workIndex / a.cols;
        int j = workIndex % a.cols;

        // Use a local variable for the dot product to avoid repeated slow global memory writes
        float sum = 0.0f;

        for (int k = 0; k < a.cols; k++) {
            float a_val = a.ptr[i * a.cols + k];
            float b_val = bT.ptr[j * bT.cols + k];
            sum += a_val * b_val;
        }

        // Write the final result once to global memory
        c.ptr[i * c.cols + j] = sum;
    }
}
