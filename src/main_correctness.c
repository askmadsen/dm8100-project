#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

#include "matrix.h"
#include "serial.h"
#include "openmp.h"

#define TOLERANCE 1e-12

int main(int argc, char **argv) {
    if (argc != 5) {
        fprintf(stderr, "usage: %s <method> <dim> <scale> <threads>\n", argv[0]);
        return 1;
    }

    char *method  = argv[1];
    int dim       = atoi(argv[2]);
    int scale     = atoi(argv[3]);
    int threads   = atoi(argv[4]);

    omp_set_num_threads(threads);

    Matrix a = matrix_create(dim, dim, scale);
    Matrix b = matrix_create(dim, dim, scale * 7);
    matrix_transpose(b);
    Matrix c_ref = matrix_create_zeros(dim, dim);
    Matrix c_test = matrix_create_zeros(dim, dim);

    matmul_transposed(a, b, c_ref);

    if (strcmp(method, "openmp") == 0) {
        matmul_openmp_transposed(a, b, c_test);
    }
    /*
    else if (strcmp(method, "mpi") == 0) {
        matmul_mpi(a, b, c_test);
    }
    */
    else {
        fprintf(stderr, "Unknown method: %s\n", method);
        return 1;
    }

    double diff = matrix_frobenius_norm(c_ref, c_test);
    int status = (diff < TOLERANCE) ? 0 : 1;

    printf("%s (diff: %e)\n", status == 0 ? "PASS" : "FAIL", diff);

    matrix_free(a);
    matrix_free(b);
    matrix_free(c_ref);
    matrix_free(c_test);

    return status;
}
