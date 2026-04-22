#include <stdio.h>

#include "matrix.h"
#include "arg_parser.h"


#define TOLERANCE 1e-12

int parse_args(int argc, char** argv, char** path_a, char** path_b) {

    ARG_INIT()
    STR_ARG(path_a)
    STR_ARG(path_b)

    return 0;
}

int main(int argc, char **argv) {
    char* path_a = NULL;
    char* path_b = NULL;
    Matrix a;
    Matrix b;

    parse_args(argc, argv, &path_a, &path_b);
    matrix_load(&a, path_a);
    matrix_load(&b, path_b);

    double norm = matrix_frobenius_norm(a, b);

    for (int i = 0; i < a.rows; i++) {
        for (int j = 0; j < a.cols; j++) {
            float va = *matrix_index(a, i, j);
            float vb = *matrix_index(b, i, j);
            if (va != vb) {
                printf("%d %d: %f %f %f\n", i, j, va, vb, vb - va);
            }
        }
    }

    int status = norm < TOLERANCE ? 0 : 1;
    return status;
}
