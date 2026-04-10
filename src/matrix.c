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
            m.ptr[i * m.cols + j] = i * scale + j;
        }
    }
}

Matrix matrix_calloc(size_t rows, size_t cols) {
    DATA* ptr = calloc(rows * cols, sizeof(DATA));
    return matrix_new(ptr, rows, cols);
}

Matrix matrix_filled(size_t rows, size_t cols, int scale) {
    Matrix m = matrix_calloc(rows, cols);
    matrix_fill(m, scale);
    return m;
}

void matrix_free(Matrix m) {
    free(m.ptr);
}

int matrix_save(Matrix m, FILE* file) {
    fprintf(file, "%ld;", m.rows);
    fprintf(file, "%ld;", m.cols);
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            fprintf(file, "%d;", *matrix_index(m, i, j));
        }
    }
    return 0;
}

int matrix_load(Matrix* m, FILE* file) {
    size_t rows;
    size_t cols;
    fscanf(file, "%ld;", &rows);  
    fscanf(file, "%ld;", &cols);
    *m = matrix_calloc(rows, cols);
    for (size_t i = 0; i < m->rows; i++) {
        for (size_t j = 0; j < m->cols; j++) {
            fscanf(file, "%d;", matrix_index(*m, i, j));
        }
    }
    return 0;
}
