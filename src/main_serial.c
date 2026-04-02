#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "matrix.h"
#include "serial.h"

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "usage: main_serial <dimension> <scale>\n");
        return 1;
    }

    int dim   = atoi(argv[1]);
    int scale = atoi(argv[2]);

    struct timespec start, end;

    Matrix a = matrix_create(dim, dim, scale);
    Matrix b = matrix_create(dim, dim, scale * 7);
    matrix_transpose(b);
    Matrix c = matrix_create_zeros(dim, dim);

    clock_gettime(CLOCK_MONOTONIC, &start);
    matmul_transposed(a, b, c);
    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    return 0;
}
