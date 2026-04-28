#include "matrix.h"

#include <stddef.h>
#include <stdio.h>

void matmul(Matrix a, Matrix b, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, k, j);
            }
        }
    }
}

void matmul_transposed(Matrix a, Matrix bT, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(bT, j, k);
            }
        }
    }
}

void matmul_chunks_inner(
    float* a, float* b, float* c,
    int m, int n, int p,
    int a_cols, int b_cols, int c_cols
) {
    if (m + n + p <= 48) {
        for (int i = 0; i < m; i++) {
            for (int k = 0; k < p; k++) {
                float r = a[i * a_cols + k];
                for (int j = 0; j < n; j++) {
                    c[i * c_cols + j] += r * b[k * b_cols + j];
                }
            }
        }
    } else {
        int m2 = m / 2;
        int n2 = n / 2;
        int p2 = p / 2;
        float* a1 = a;
        float* a2 = a + n2;
        float* a3 = a + m2 * a_cols;
        float* a4 = a + m2 * a_cols + n2;
        float* b1 = b;
        float* b2 = b + p2;
        float* b3 = b + n2 * b_cols;
        float* b4 = b + n2 * b_cols + p2;
        float* c1 = c;
        float* c2 = c + p2;
        float* c3 = c + m2 * c_cols;
        float* c4 = c + m2 * c_cols + p2;

        matmul_chunks_inner(a1, b1, c1, m2, n2, p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a2, b3, c1, m2, n - n2, p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a1, b2, c2, m2, n2, p - p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a2, b4, c2, m2, n - n2, p - p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a3, b1, c3, m - m2, n2, p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a4, b2, c3, m - m2, n - n2, p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a3, b2, c4, m - m2, n2, p - p2, a_cols, b_cols, c_cols);
        matmul_chunks_inner(a4, b4, c4, m - m2, n - n2, p - p2, a_cols, b_cols, c_cols);
    }
}

void matmul_chunks(Matrix a, Matrix b, Matrix c) {
    matmul_chunks_inner(a.ptr, b.ptr, c.ptr, a.rows, b.rows, c.cols, a.cols, b.cols, c.cols);
}