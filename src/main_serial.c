#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "matrix.h"
#include "serial.h"
#include "arg_parser.h"


int parse_args(int argc, char** argv, int* dim, char** dest) {

    ARG_INIT()
    INT_ARG(dim)

    OPT_ARGS() {
        OPT_STR_ARG("--dest", dest)
        OPT_END()
    }

    return 0;
}

int main(int argc, char **argv) {
    int dim;
    char* dest = NULL;
    parse_args(argc, argv, &dim, &dest);

    Matrix a = matrix_filled(dim, dim, 7);
    Matrix b = matrix_filled(dim, dim, 13);
    Matrix c = matrix_calloc(dim, dim);

    matrix_save(a, "a.txt");
    matrix_save(up(a), "up.txt");
    matrix_save(down(a), "down.txt");
    matrix_save(left(a), "left.txt");
    matrix_save(right(a), "right.txt");

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    // matrix_transpose(b);
    matmul_chunks2(a, b, c);

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
