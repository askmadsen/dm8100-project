#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omp.h>

#include "matrix.h"
#include "openmp.h"
#include "arg_parser.h"


int parse_args(int argc, char** argv, int* dim, int* num_threads, char** dest) {

    ARG_INIT()
    INT_ARG(dim)
    INT_ARG(num_threads)

    OPT_START()
    OPT_STR_ARG("--dest", dest)
    OPT_END()

    return 0;
}

int main(int argc, char **argv) {

    int dim;
    int num_threads;
    char* dest = NULL;

    parse_args(argc, argv, &dim, &num_threads, &dest);

    omp_set_num_threads(num_threads);

    Matrix a = matrix_filled(dim, dim, 7);
    Matrix b = matrix_filled(dim, dim, 13);
    Matrix c = matrix_calloc(dim, dim);


    struct timespec start, end;

    //matrix_transpose(b);

    clock_gettime(CLOCK_MONOTONIC, &start);

    //matmul_openmp_transposed(a, b, c);

    matmul_openmp(a, b, c);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    if (dest != NULL) {
        FILE* file = fopen(dest, "w");
        matrix_save(c, file);
    }

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    return 0;
}
