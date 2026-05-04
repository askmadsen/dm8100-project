#include "matrix.h"

#include <stddef.h>
#include <stdio.h>

void matmul(Matrix a, Matrix b, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t k = 0; k < a.cols; k++) {
            float  entry= *matrix_index(a, i, k);
            for (size_t j = 0; j < c.cols; j++) {
                *matrix_index(c, i, j) += entry * *matrix_index(b, k, j);
            }
        }
    }
}

void matmul_transposed(Matrix a, Matrix bT, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            float sum = 0;
            for (size_t k = 0; k < a.cols; k++) {
                sum += *matrix_index(a, i, k) * *matrix_index(bT, j, k);
            }
            *matrix_index(c, i, j) = sum;
        }
    }
}

void matmul_chunks2(Matrix a, Matrix b, Matrix c) {
    if (a.rows + b.rows + c.cols <= 48) {
        matmul(a, b, c);
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

        matmul_chunks2(a1, b1, c1);
        matmul_chunks2(a2, b3, c1);
        matmul_chunks2(a1, b2, c2);
        matmul_chunks2(a2, b4, c2);
        matmul_chunks2(a3, b1, c3);
        matmul_chunks2(a4, b3, c3);
        matmul_chunks2(a3, b2, c4);
        matmul_chunks2(a4, b4, c4);
    }
}
