#include "matrix.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>

void matrix_display(Matrix m) {
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            printf("%15.1f", *matrix_index(m, i, j));
        }
        printf("\n");
    }
}

double matrix_frobenius_norm(Matrix m, Matrix n) {
    double sum = 0.0;
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            double diff = *matrix_index(m, i, j) - *matrix_index(n, i, j);
            sum += diff * diff;
        }
    }
    return sum;
}

void matrix_transpose(Matrix m) {
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = i + 1; j < m.cols; j++) {
            DATA temp = *matrix_index(m, i, j);
            *matrix_index(m, i, j) = *matrix_index(m, j, i);
            *matrix_index(m, j, i) = temp;
        }
    }
}

Matrix create_matrix(size_t rows, size_t cols, int scale) {
    DATA* data = malloc(rows * cols * sizeof(DATA));

    for (size_t i = 0; i < rows; i++) {
        for (size_t j = 0; j < cols; j++) {
            data[i * cols + j] = i * scale + j;
        }
    }

    return matrix_new(data, rows, cols);
}

Matrix create_zeros_matrix(size_t rows, size_t cols) {
    DATA* data = calloc(rows * cols, sizeof(DATA));
    return matrix_new(data, rows, cols);
}

void free_matrix(Matrix m) {
    free(m.ptr);
}
