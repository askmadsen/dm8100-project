#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "cuda.h"

int parse_args(int argc, char** argv, int* dim, int* threads, int* blocks, char** dest) {
    if (argc < 4) {
        return -1;
    }

    *dim     = atoi(argv[1]);
    *threads = atoi(argv[2]);
    *blocks  = atoi(argv[3]);


    int i = 4;
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
    int threads;
    int blocks;
    char* dest = NULL;
    parse_args(argc, argv, &dim, &threads, &blocks, &dest);

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


    matrix_transpose(b);

    cudaMemcpy(a_dev.ptr, a.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(b_dev.ptr, b.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemset(c_dev.ptr, 0, dim*dim*sizeof(float));

    //int blocks = dim * dim / 1024 + 1;
    //int threads = dim * dim < 1024 ? dim * dim : 1024;

    clock_gettime(CLOCK_MONOTONIC, &start);


    matmul_cuda_transposed<<<blocks, threads>>>(a_dev, b_dev, c_dev);

    cudaDeviceSynchronize();

    clock_gettime(CLOCK_MONOTONIC, &end);


    cudaMemcpy(c.ptr, c_dev.ptr, dim*dim*sizeof(float), cudaMemcpyDeviceToHost);


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
