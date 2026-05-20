#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

typedef float DATA;

typedef struct Matrix {
    DATA* ptr;
    int rows;
    int cols;
    int stride;
} Matrix;

static inline Matrix matrix_new(DATA* ptr, int rows, int cols) {
    return (Matrix) { ptr, rows, cols, cols };
}

static inline DATA* matrix_index(Matrix m, int i, int j) {
    return m.ptr + i * m.stride + j;
}

void matrix_display(Matrix m);

double matrix_frobenius_norm(Matrix m, Matrix n);

void matrix_transpose(Matrix m);

Matrix matrix_calloc(int rows, int cols);

void matrix_fill(Matrix m, int scale);

Matrix matrix_filled(int rows, int cols, int scale);

void matrix_free(Matrix m);

int matrix_save(Matrix m, char* file_path);

int matrix_load(Matrix* m, char* file_path);

Matrix up_at(Matrix m, int i);
Matrix down_at(Matrix m, int i);
Matrix left_at(Matrix m, int j);
Matrix right_at(Matrix m, int j);

Matrix up(Matrix m);
Matrix down(Matrix m);
Matrix left(Matrix m);
Matrix right(Matrix m);

#ifdef __cplusplus
}
#endif
