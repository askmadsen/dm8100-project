#pragma once

#include <stddef.h>

typedef float DATA;

typedef struct Matrix {
    DATA* ptr;
    size_t rows;
    size_t cols;
} Matrix;

static inline Matrix matrix_new(DATA* ptr, size_t rows, size_t cols) {
    return (Matrix) { ptr, rows, cols };
}

static inline DATA* matrix_index(Matrix m, size_t i, size_t j) {
    return m.ptr + i * m.cols + j;
}

void matrix_display(Matrix m);

double matrix_frobenius_norm(Matrix m, Matrix n);

void matrix_transpose(Matrix m);

Matrix matrix_create(size_t rows, size_t cols, int scale);

Matrix matrix_create_zeros(size_t rows, size_t cols);

void matrix_free(Matrix m);
