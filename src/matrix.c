#include "matrix.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

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
    return sqrt(sum);
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

void matrix_fill(Matrix m, int scale) {
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            m.data[i * m.cols + j] = i * scale + j;
        }
    }
}
