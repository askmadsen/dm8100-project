#include "openmp.h"
#include "serial.h"

#include <omp.h>

void matmul_openmp(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (int i = 0; i < c.rows; i++) {
        for (int k = 0; k < a.cols; k++) {
            float entry = *matrix_index(a, i, k);
            for (int j = 0; j < c.cols; j++) {
                *matrix_index(c, i, j) += entry * *matrix_index(b, k, j);
            }
        }
    }
}

void matmul_openmp_transposed(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for
    for (int i = 0; i < c.rows; i++) {
        for (int j = 0; j < c.cols; j++) {
            float sum = 0;
            for (int k = 0; k < a.cols; k++) {
                sum += *matrix_index(a, i, k) * *matrix_index(b, j, k);
            }
            *matrix_index(c, i, j) = sum;
        }
    }
}

void matmul_openmp_chunks(Matrix a, Matrix b, Matrix c, int threshold) {
    if (a.rows + b.rows + c.cols <= threshold) {
        matmul_openmp(a, b, c);
    } else {
        Matrix a1 = up(left(a));
        Matrix a2 = up(right(a));
        Matrix a3 = down(left(a));
        Matrix a4 = down(right(a));
        Matrix b1 = up(left(b));
        Matrix b2 = up(right(b));
        Matrix b3 = down(left(b));
        Matrix b4 = down(right(b));
        Matrix c1 = up(left(c));
        Matrix c2 = up(right(c));
        Matrix c3 = down(left(c));
        Matrix c4 = down(right(c));

        matmul_openmp_chunks(a1, b1, c1, threshold);
        matmul_openmp_chunks(a2, b3, c1, threshold);
        matmul_openmp_chunks(a1, b2, c2, threshold);
        matmul_openmp_chunks(a2, b4, c2, threshold);
        matmul_openmp_chunks(a3, b1, c3, threshold);
        matmul_openmp_chunks(a4, b3, c3, threshold);
        matmul_openmp_chunks(a3, b2, c4, threshold);
        matmul_openmp_chunks(a4, b4, c4, threshold);
    }
}