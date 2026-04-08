#include <stdio.h>

#include "matrix.h"
#include "cuda.h"

int main () {

    int dim = 64;


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

    cudaMemcpy(a_dev.data, a.data, dim*dim*sizeof(float), cudaMemcpyDefault);
    cudaMemcpy(b_dev.data, b.data, dim*dim*sizeof(float), cudaMemcpyDefault);
    cudaMemset(c_dev.data, 0, dim*dim*sizeof(float));

    matmul_cuda_transposed<<<64, 64>>>(a_dev, b_dev, c_dev);

    cudaDeviceSynchronize();

    cudaMemcpy(c.data, c_dev.data, dim*dim*sizeof(float), cudaMemcpyDefault);


    matrix_display(c);


}
