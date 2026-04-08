#include <stdio.h>

#include "matrix.h"
#include "cuda.h"

int main () {

    int dim = 4;

    float* a_ptr;
    float* b_ptr;
    float* c_ptr;

    float* a_dev_ptr;
    float* b_dev_ptr;
    float* c_dev_ptr;

    cudaMallocHost(&a_ptr, sizeof(float)*dim*dim);
    cudaMallocHost(&b_ptr, sizeof(float)*dim*dim);
    cudaMallocHost(&c_ptr, sizeof(float)*dim*dim);

    Matrix a = matrix_new(a_ptr, dim, dim);
    Matrix b = matrix_new(b_ptr, dim, dim);
    Matrix c = matrix_new(c_ptr, dim, dim);

    cudaMalloc(&a_dev_ptr, sizeof(float)*dim*dim);
    cudaMalloc(&b_dev_ptr, sizeof(float)*dim*dim);
    cudaMalloc(&c_dev_ptr, sizeof(float)*dim*dim);

    Matrix a_dev = matrix_new(a_dev_ptr, dim, dim);
    Matrix b_dev = matrix_new(b_dev_ptr, dim, dim);
    Matrix c_dev = matrix_new(c_dev_ptr, dim, dim);

    matrix_fill(a, 7);
    matrix_fill(b, 13);

    matrix_transpose(b);

    cudaMemcpy(a_dev.ptr, a.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(b_dev.ptr, b.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemset(c_dev.ptr, 0, dim*dim*sizeof(float));

    matmul_cuda_transposed<<<dim, dim>>>(a_dev, b_dev, c_dev);

    cudaDeviceSynchronize();

    cudaMemcpy(c.ptr, c_dev.ptr, dim*dim*sizeof(float), cudaMemcpyDeviceToHost);

    cudaError_t error = cudaGetLastError();
    const char* error_str = cudaGetErrorString(error);
    printf("%s\n", error_str);

    matrix_display(a);
    matrix_display(b);
    matrix_display(c);


}
