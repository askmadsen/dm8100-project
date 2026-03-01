#include "matrix.h"
#include "openmp.h"

#include <stddef.h>
#include <stdio.h>
#include <omp.h>

void matrix_multiply_serial(Matrix a, Matrix b, Matrix c);

int main() {
    float a_data[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
    };
    float b_data[9] = {
        0, 1, 0,
        1, 0, 1,
        0, 1, 0,
    };
    float c_data_serial[9];
    for (int i = 0; i < 9; i++) {
        c_data_serial[i] = 0;
    }

    float c_data_openmp[9];
    for (int i = 0; i < 9; i++) {
        c_data_openmp[i] = 0;
    }

    Matrix a = matrix_new(a_data, 3, 3);
    Matrix b = matrix_new(b_data, 3, 3);
    Matrix c_serial = matrix_new(c_data_serial, 3, 3);
    Matrix c_openmp = matrix_new(c_data_openmp, 3, 3);

    matrix_multiply_serial(a, b, c_serial);
    matrix_display(c_serial);
    printf("\n");
    matrix_multiply_openmp(a, b, c_openmp);
    matrix_display(c_openmp);

    double frobenius_norm = matrix_frobenius_norm(c_serial, c_openmp);
    printf("Frobenius norm of the difference between serial and openmp results: %f\n", frobenius_norm);

    Matrix new_a = create_matrix(1024, 1024, 2);
    Matrix new_b = create_matrix(1024, 1024, 5);
    Matrix new_c_serial = create_zeros_matrix(1024, 1024);
    Matrix new_c_openmp = create_zeros_matrix(1024, 1024);

    double start = omp_get_wtime();
    matrix_multiply_serial(new_a, new_b, new_c_serial);
    double end = omp_get_wtime();
    printf("Time taken for serial multiplication of large matrices: %f seconds\n", end - start);
    start = omp_get_wtime();
    matrix_multiply_openmp(new_a, new_b, new_c_openmp);
    end = omp_get_wtime();
    printf("Time taken for openmp multiplication of large matrices: %f seconds\n", end - start);
    frobenius_norm = matrix_frobenius_norm(new_c_serial, new_c_openmp);
    printf("Frobenius norm of the difference between serial and openmp results for large matrices: %f\n", frobenius_norm);
    free_matrix(new_a);
    free_matrix(new_b);
    free_matrix(new_c_serial);
    free_matrix(new_c_openmp);
}

void matrix_multiply_serial(Matrix a, Matrix b, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, k, j);
            }
        }
    }
}
