#include <stdio.h>

#include "matrix.h"

#define TOLERANCE 1e-12

int parse_args(int argc, char** argv, Matrix* a, Matrix* b) {
    if (argc < 3) {
        return -1;
    }

    FILE* file_a = fopen(argv[1], "r");
    matrix_load(a, file_a);
    fclose(file_a);

    FILE* file_b = fopen(argv[2], "r");
    matrix_load(b, file_b);
    fclose(file_b);

    return 0;
}

int main(int argc, char **argv) {
    Matrix a;
    Matrix b;
    parse_args(argc, argv, &a, &b);
    double norm = matrix_frobenius_norm(a, b);
    int status = norm < TOLERANCE ? 0 : 1;
    return status;
}
