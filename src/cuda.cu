#include <cuda_runtime.h>

#include "matrix.h"


__device__ typedef struct MatrixChunk {
    int row_start;
    int row_end;
    int col_start;
    int col_end;
} MatrixChunk;

__device__ MatrixChunk chunk_from_index(int i, int rows, int cols, int chunk_size) {
    int row = i * chunk_size / rows * chunk_size;
    int col = i * chunk_size % rows;
    int row_end = rows - row >= chunk_size ? row + chunk_size : rows;
    int col_end = cols - col >= chunk_size ? col + chunk_size : cols;

    return (MatrixChunk) {
        .row_start = row,
        .row_end = row_end,
        .col_start = col,
        .col_end = col_end
    };
}

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

__global__ void matmul_cuda_chunks(Matrix a, Matrix b, Matrix c, int chunk_size) {
    int total_threads = blockDim.x * gridDim.x;
    int work_index = threadIdx.x + blockDim.x * blockIdx.x;
    int chunk_count = ((c.rows - 1) / chunk_size + 1) * ((c.cols - 1) / chunk_size + 1);

    for (int chunk_index = work_index; chunk_index < chunk_count; chunk_index += total_threads) {
        MatrixChunk chunk = chunk_from_index(chunk_index, c.rows, c.cols, chunk_size);
        for (int i = chunk.row_start; i < chunk.row_end; i++) {
            for (int k = 0; k < a.cols; k++) {
                float entry = a.ptr[i * a.cols+ k]; //*matrix_index(a, i, k);
                for (int j = chunk.col_start; j < chunk.col_end; j++) {
                    c.ptr[i * c.cols + j] += entry * b.ptr[k * b.cols + j]; // *matrix_index(b, k, j);
                }
            }
        }
    }
}
