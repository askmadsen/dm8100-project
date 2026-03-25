#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>

#include "matrix.h"
#include "openmp.h"

int main(int argc, char **argv) {
    if (argc != 4) {
        fprintf(stderr, "usage: main_openmp <dimension> <scale> <num_threads> \n");
        return 1;
    }

    int dim   = atoi(argv[1]);
    int scale = atoi(argv[2]);
    int num_threads = atoi(argv[3]);

    omp_set_num_threads(num_threads);

    struct timespec start, end;

    Matrix a = matrix_create(dim, dim, scale);
    Matrix b = matrix_create(dim, dim, scale * 7);
    Matrix c = matrix_create_zeros(dim, dim);

    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul_openmp_transposed(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    return 0;
}
