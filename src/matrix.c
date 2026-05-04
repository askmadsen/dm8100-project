#include "matrix.h"

#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <math.h>

void matrix_display(Matrix m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            printf("%15.1f", *matrix_index(m, i, j));
        }
        printf("\n");
    }
}

double matrix_frobenius_norm(Matrix m, Matrix n) {
    double sum = 0.0;
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            double diff = *matrix_index(m, i, j) - *matrix_index(n, i, j);
            sum += diff * diff;
        }
    }
    return sqrt(sum);
}

void matrix_transpose(Matrix m) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = i + 1; j < m.cols; j++) {
            DATA temp = *matrix_index(m, i, j);
            *matrix_index(m, i, j) = *matrix_index(m, j, i);
            *matrix_index(m, j, i) = temp;
        }
    }
}

void matrix_fill(Matrix m, int scale) {
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            m.ptr[i * m.cols + j] = (i * scale + j) % (scale * scale);
        }
    }
}

Matrix matrix_calloc(int rows, int cols) {
    DATA* ptr = calloc(rows * cols, sizeof(DATA));
    return matrix_new(ptr, rows, cols);
}

Matrix matrix_filled(int rows, int cols, int scale) {
    Matrix m = matrix_calloc(rows, cols);
    matrix_fill(m, scale);
    return m;
}

void matrix_free(Matrix m) {
    free(m.ptr);
}

int matrix_save(Matrix m, char* file_path) {
    FILE* file = fopen(file_path, "w");
    if (!file) {
        return -1;
    }
    
    fprintf(file, "%d;", m.rows);
    fprintf(file, "%d;", m.cols);
    for (int i = 0; i < m.rows; i++) {
        for (int j = 0; j < m.cols; j++) {
            fprintf(file, "%f;", *matrix_index(m, i, j));
        }
    }

    fclose(file);
    return 0;
}

int matrix_load(Matrix* m, char* file_path) {
    FILE* file = fopen(file_path, "r");
    if (!file) {
        return -1;
    }

    int rows;
    int cols;
    fscanf(file, "%d;", &rows);
    fscanf(file, "%d;", &cols);
    *m = matrix_calloc(rows, cols);
    for (int i = 0; i < m->rows; i++) {
        for (int j = 0; j < m->cols; j++) {
            fscanf(file, "%f;", matrix_index(*m, i, j));
        }
    }
    
    fclose(file);
    return 0;
}

Matrix up(Matrix m) {
    return (Matrix) { m.ptr, m.rows / 2, m.cols, m.stride };
}

Matrix down(Matrix m) {
    return (Matrix) { m.ptr + m.rows / 2 * m.stride, m.rows - m.rows / 2, m.cols, m.stride };
}

Matrix left(Matrix m) {
    return (Matrix) { m.ptr, m.rows, m.cols / 2, m.stride };
}

Matrix right(Matrix m) {
    return (Matrix) { m.ptr + m.cols / 2, m.rows, m.cols - m.cols / 2, m.stride };
}
