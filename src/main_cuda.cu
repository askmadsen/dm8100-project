#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "cuda.h"
#include "arg_parser.h"

#define TILE 32

int parse_args(int argc, char** argv, int* dim, int* threads, int* blocks, int* chunk_size, char** dest, const char** alg) {
    ARG_INIT()
    INT_ARG(dim)

    OPT_ARGS(
        OPT_STR_ARG("--dest", dest)
        OPT_INT_ARG("--threads", threads)
        OPT_INT_ARG("--blocks", blocks)
        OPT_INT_ARG("--chunk_size", chunk_size)
        OPT_STR_ARG("--alg", alg)
    )

    return 0;
}


int main(int argc, char** argv) {
    int device;
    struct cudaDeviceProp prop;

    cudaError_t err = cudaGetDevice(&device);
    if (err != cudaSuccess) {
        printf("Error getting device: %s\n", cudaGetErrorString(err));
        return 1;
    }

    cudaGetDeviceProperties(&prop, device);

    int dim;
    int threads = 256;
    int blocks = prop.multiProcessorCount;
    int chunk_size = 4;
    char* dest = NULL;
    const char* alg = "blocks";

    parse_args(argc, argv, &dim, &threads, &blocks, &chunk_size, &dest, &alg);

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

    cudaMemcpy(a_dev.ptr, a.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(b_dev.ptr, b.ptr, dim*dim*sizeof(float), cudaMemcpyHostToDevice);
    cudaMemset(c_dev.ptr, 0, dim*dim*sizeof(float));

    clock_gettime(CLOCK_MONOTONIC, &start);

    if (!strcmp(alg, "chunks") || !strcmp(alg, "matmul_cuda_chunks")) {
        matmul_cuda_chunks<<<blocks, threads>>>(a_dev, b_dev, c_dev, chunk_size);
    } else if (!strcmp(alg, "transposed") || !strcmp(alg, "matmul_cuda_transposed")) {
        matrix_transpose(b);
        matmul_cuda_transposed<<<blocks, threads>>>(a_dev, b_dev, c_dev);
    } else if (!strcmp(alg, "blocks") || !strcmp(alg, "matmul_cuda_blocks")) {
        dim3 block(TILE, TILE);
        dim3 grid(
            (dim + TILE - 1) / TILE,
            (dim + TILE - 1) / TILE
        );
        matmul_cuda_blocks<<<grid, block>>>(a_dev, b_dev, c_dev);
    } else {
        fprintf(stderr, "Matmul algorithm %s is not implemented \n", alg);
        return -1;
    }

    cudaDeviceSynchronize();

    clock_gettime(CLOCK_MONOTONIC, &end);


    cudaMemcpy(c.ptr, c_dev.ptr, dim*dim*sizeof(float), cudaMemcpyDeviceToHost);


    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%f\n", elapsed);

    if (dest != NULL) {
        matrix_save(c, dest);
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
