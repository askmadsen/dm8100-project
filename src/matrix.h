#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdio.h>

typedef float DATA;

// A simple matrix structure that holds a pointer to the data, dimensions, and stride for indexing.
typedef struct Matrix {
    DATA* ptr;
    int rows;
    int cols;
    int stride;
} Matrix;

// Creates a new matrix with the given data pointer, dimensions, and stride.
static inline Matrix matrix_new(DATA* ptr, int rows, int cols) {
    return (Matrix) { ptr, rows, cols, cols };
}

// Returns a pointer to the element at row i and column j of the matrix, accounting for stride.
static inline DATA* matrix_index(Matrix m, int i, int j) {
    return m.ptr + i * m.stride + j;
}

// Displays the matrix to standard output with fixed-width formatting.
void matrix_display(Matrix m);

// Computes the Frobenius norm of the difference between two matrices, used for correctness checks.
double matrix_frobenius_norm(Matrix m, Matrix n);

// Transposes the matrix in place by swapping elements across the diagonal.
void matrix_transpose(Matrix m);

// Allocates a new matrix with the given dimensions, initialized to zero.
Matrix matrix_calloc(int rows, int cols);

// Fills the matrix with values based on the provided scale.
void matrix_fill(Matrix m, int scale);

// Allocates a new matrix with the given dimensions, filled with values based on the provided scale.
Matrix matrix_filled(int rows, int cols, int scale);

// Frees the memory allocated for the matrix's data pointer.
void matrix_free(Matrix m);

// Saves the matrix to a file in a simple text format with dimensions followed by values.
int matrix_save(Matrix m, char* file_path);

// Loads a matrix from a file in the same format as saved by matrix_save, allocating memory for the data.
int matrix_load(Matrix* m, char* file_path);

// Returns a submatrix view of the original matrix ending at row i, with the same number of columns.
Matrix up_at(Matrix m, int i);

// Returns a submatrix view of the original matrix starting from row i, with the same number of columns.
Matrix down_at(Matrix m, int i);

// Returns a submatrix view of the original matrix ending at column j, with the same number of rows.
Matrix left_at(Matrix m, int j);

// Returns a submatrix view of the original matrix starting from column j, with the same number of rows.
Matrix right_at(Matrix m, int j);

// Returns a submatrix view of the original matrix containing the top half of the rows, with the same number of columns.
Matrix up(Matrix m);

// Returns a submatrix view of the original matrix containing the bottom half of the rows, with the same number of columns.
Matrix down(Matrix m);

// Returns a submatrix view of the original matrix containing the left half of the columns, with the same number of rows.
Matrix left(Matrix m);

// Returns a submatrix view of the original matrix containing the right half of the columns, with the same number of rows.
Matrix right(Matrix m);

#ifdef __cplusplus
}
#endif
