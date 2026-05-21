#include "openmp.h"
#include "serial.h"

#include <omp.h>
#include <string.h>

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

void matmul_openmp_recursive(Matrix a, Matrix b, Matrix c, int threshold) {
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

        matmul_openmp_recursive(a1, b1, c1, threshold);
        matmul_openmp_recursive(a2, b3, c1, threshold);
        matmul_openmp_recursive(a1, b2, c2, threshold);
        matmul_openmp_recursive(a2, b4, c2, threshold);
        matmul_openmp_recursive(a3, b1, c3, threshold);
        matmul_openmp_recursive(a4, b3, c3, threshold);
        matmul_openmp_recursive(a3, b2, c4, threshold);
        matmul_openmp_recursive(a4, b4, c4, threshold);
    }
}

#define BLOCK_LEN 256

void matmul_openmp_block(Matrix a, Matrix b, Matrix c) {
    float block_a[BLOCK_LEN][BLOCK_LEN];
    float block_b[BLOCK_LEN][BLOCK_LEN];
    float block_c[BLOCK_LEN][BLOCK_LEN];
    int split;
    size_t remaining_bytes_cols;
    size_t remaining_bytes_rows;

    split = a.cols < BLOCK_LEN ? a.cols : BLOCK_LEN;
    remaining_bytes_cols = (BLOCK_LEN - split) * sizeof(float);
    remaining_bytes_rows = (BLOCK_LEN - a.rows) * BLOCK_LEN * sizeof(float);
    for (int i = 0; i < a.rows; i++) {
        float* src = a.ptr + i * a.stride;
        float* dest = (float*)block_a + i * BLOCK_LEN;
        memcpy(dest, src, split * sizeof(float));
        memset(dest + split, 0, remaining_bytes_cols);
    }
    memset((float*)block_a + a.rows * BLOCK_LEN, 0, remaining_bytes_rows);

    split = b.cols < BLOCK_LEN ? b.cols : BLOCK_LEN;
    remaining_bytes_cols = (BLOCK_LEN - split) * sizeof(float);
    remaining_bytes_rows = (BLOCK_LEN - b.rows) * BLOCK_LEN * sizeof(float);
    for (int i = 0; i < b.rows; i++) {
        float* src = b.ptr + i * b.stride;
        float* dest = (float*)block_b + i * BLOCK_LEN;
        memcpy(dest, src, split * sizeof(float));
        memset(dest + split, 0, remaining_bytes_cols);
    }
    memset((float*)block_b + b.rows * BLOCK_LEN, 0, remaining_bytes_rows);

    for (int i = 0; i < BLOCK_LEN; i++) {
        for (int k = 0; k < BLOCK_LEN; k++) {
            float a_ik = block_a[i][k];
            for (int j = 0; j < BLOCK_LEN; j++) {
                block_c[i][j] += a_ik * block_b[k][j];
            }
        }
    }

    for (int i = 0; i < c.rows; i++) {
        for (int j = 0; j < c.cols; j++) {
            *matrix_index(c, i, j) = block_c[i][j];
        }
    }
}

void matmul_openmp_blocks(Matrix a, Matrix b, Matrix c) {
    #pragma omp parallel for collapse(2)
    for (int i = 0; i < c.rows; i += BLOCK_LEN) {
        for (int j = 0; j < c.cols; j += BLOCK_LEN) {
            for (int k = 0; k < b.rows; k += BLOCK_LEN) {
                int rows = c.rows - i < BLOCK_LEN ? c.rows - i : BLOCK_LEN;
                int cols = c.cols - j < BLOCK_LEN ? c.cols - j : BLOCK_LEN;
                int len = b.rows - k < BLOCK_LEN ? b.rows - k : BLOCK_LEN;
                Matrix a_ik = left_at(right_at(up_at(down_at(a, i), rows), k), len);
                Matrix b_kj = left_at(right_at(up_at(down_at(b, k), len), j), cols);
                Matrix c_ij = left_at(right_at(up_at(down_at(c, i), rows), j), cols);
                matmul_openmp_block(a_ik, b_kj, c_ij);
            }
        }
    }
}