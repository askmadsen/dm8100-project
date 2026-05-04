#include "openmp.h"

#include <omp.h>

void matmul_openmp(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t k = 0; k < a.cols; k++) {
            float entry = *matrix_index(a, i, k);
            for (size_t j = 0; j < c.cols; j++) {
                *matrix_index(c, i, j) += entry * *matrix_index(b, k, j);
            }
        }
    }
}

void matmul_openmp_transposed(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            float sum = 0;
            for (size_t k = 0; k < a.cols; k++) {
                sum += *matrix_index(a, i, k) * *matrix_index(b, j, k);
            }
            *matrix_index(c, i, j) = sum;
        }
    }
}
