#include <stdio.h>
#include <time.h>
#include <omp.h>

#include "matrix.h"
#include "openmp.h"
#include "arg_parser.h"


int parse_args(int argc, char** argv, int* dim, int* num_threads, char** dest, const char** alg, int* threshold) {
    ARG_INIT()
    INT_ARG(dim)
    INT_ARG(num_threads)

    OPT_ARGS(
        OPT_STR_ARG("--dest", dest)
        OPT_STR_ARG("--alg", alg)
        OPT_INT_ARG("--threshold", threshold)
    )

    return 0;
}

int main(int argc, char **argv) {

    int dim;
    int num_threads;
    char* dest = NULL;
    const char* alg = "blocks";
    int threshold = 1024;

    CONTEXT(
        parse_args(argc, argv, &dim, &num_threads, &dest, &alg, &threshold), 
        fprintf(stderr, "While parsing arguments for main_openmp\n")
    );

    omp_set_num_threads(num_threads);

    Matrix a = matrix_filled(dim, dim, 7);
    Matrix b = matrix_filled(dim, dim, 13);
    Matrix c = matrix_calloc(dim, dim);


    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC, &start);

    if (!strcmp(alg, "simple") || !strcmp(alg, "matmul_openmp")) {
        matmul_openmp(a, b, c);
    } else if (!strcmp(alg, "transposed") || !strcmp(alg, "matmul_openmp_transposed")) {
        matrix_transpose(b);
        matmul_openmp_transposed(a, b, c);
    } else if (!strcmp(alg, "recursive") || !strcmp(alg, "matmul_openmp_recursive")) {
        matmul_openmp_recursive(a, b, c, threshold);
    } else if (!strcmp(alg, "blocks") || !strcmp(alg, "matmul_openmp_blocks")) {
        matmul_openmp_blocks(a, b, c);
    } else {
        fprintf(stderr, "Matmul algorithm %s is not implemented \n", alg);
        return -1;
    }

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    if (dest != NULL) {
        matrix_save(c, dest);
    }

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    return 0;
}
