#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "serial.h"
#include "arg_parser.h"


int parse_args(int argc, char** argv, int* dim, char** dest, const char** alg, int* threshold) {
    ARG_INIT()
    INT_ARG(dim)

    OPT_ARGS(
        OPT_STR_ARG("--dest", dest)
        OPT_STR_ARG("--alg", alg)
        OPT_INT_ARG("--threshold", threshold)
    )

    return 0;
}

int main(int argc, char **argv) {
    int dim;
    char* dest = NULL;
    const char* alg = "chunks";
    int threshold = 1024;

    CONTEXT(
        parse_args(argc, argv, &dim, &dest, &alg, &threshold),
        fprintf(stderr, "While parsing arguments\n")
    );

    Matrix a = matrix_filled(dim, dim, 7);
    Matrix b = matrix_filled(dim, dim, 13);
    Matrix c = matrix_calloc(dim, dim);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    if (!strcmp(alg, "ikj") || !strcmp(alg, "matmul_ikj")) {
        matmul_ikj(a, b, c);
    } else if (!strcmp(alg, "ijk") || !strcmp(alg, "matmul_ijk")) {
        matmul_ijk(a, b, c);
    } else if (!strcmp(alg, "transposed") || !strcmp(alg, "matmul_transposed")) {
        matrix_transpose(b);
        matmul_transposed(a, b, c);
    } else if (!strcmp(alg, "chunks") || !strcmp(alg, "matmul_chunks")) {
        matmul_chunks(a, b, c, threshold);
    } else {
        fprintf(stderr, "Error: No algorithm named %s\n", alg);
        return 1;
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
