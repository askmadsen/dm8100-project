#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "cuda.h"

int parse_args(int argc, char** argv, int* dim, char** dest) {
    if (argc < 2) {
        return -1;
    }

    *dim = atoi(argv[1]);

    int i = 2;
    while (i < argc) {
        if (!strcmp(argv[i], "--dest")) {
            i++;
            if (i < argc) {
                *dest = argv[i];
            }
            i++;
        }
    }

    return 0;
}

// 
// --dest <PATH>
// --output <MODE>
int main(int argc, char** argv) {
    int dim;
    char* dest = NULL;
    parse_args(argc, argv, &dim, &dest);

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

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    matrix_transpose(b);

    cudaMemcpy(a_dev.ptr, a.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(b_dev.ptr, b.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemset(c_dev.ptr, 0, dim*dim*sizeof(float));

    int blocks = dim * dim / 1024 + 1;
    int threads = dim < 1024 ? dim : 1024;

    matmul_cuda_transposed<<<blocks, threads>>>(a_dev, b_dev, c_dev);

    cudaDeviceSynchronize();

    cudaMemcpy(c.ptr, c_dev.ptr, dim*dim*sizeof(float), cudaMemcpyDeviceToHost);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%f\n", elapsed);

    if (dest != NULL) {
        FILE* file = fopen(dest, "w");
        matrix_save(c, file);
        fclose(file);
    }

    cudaError_t error = cudaGetLastError();
    if (error != 0) {
        const char* error_str = cudaGetErrorString(error);
        printf("%s\n", error_str);
    }

    cudaFree(a_dev.ptr);
    cudaFree(b_dev.ptr);
    cudaFree(c_dev.ptr);
    cudaFreeHost(a.ptr);
    cudaFreeHost(b.ptr);
    cudaFreeHost(c.ptr);
}