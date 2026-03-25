#include "openmp.h"

#include <omp.h>

void matmul_openmp(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, k, j);
            }
        }
    }
}

void matmul_openmp_transposed(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, j, k);
            }
        }
    }
}