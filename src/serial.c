#include "matrix.h"

#include <stddef.h>

void matrix_multiply_serial(Matrix a, Matrix b, Matrix c);

int main() {
    float a_data[9] = {
        1, 2, 3,
        4, 5, 6,
        7, 8, 9,
    };
    float b_data[9] = {
        0, 1, 0,
        1, 0, 1,
        0, 1, 0,
    };
    float c_data[9];
    for (int i = 0; i < 9; i++) {
        c_data[i] = 0;
    }

    Matrix a = matrix_new(a_data, 3, 3);
    Matrix b = matrix_new(b_data, 3, 3);
    Matrix c = matrix_new(c_data, 3, 3);

    matrix_multiply_serial(a, b, c);
    matrix_display(c);
}

void matrix_multiply_serial(Matrix a, Matrix b, Matrix c) {
    for (size_t i = 0; i < c.rows; i++) {
        for (size_t j = 0; j < c.cols; j++) {
            for (size_t k = 0; k < a.cols; k++) {
                *matrix_index(c, i, j) += *matrix_index(a, i, k) * *matrix_index(b, k, j);
            }
        }
    }
}