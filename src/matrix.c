#include "matrix.h"
#include <stdio.h>
#include <stddef.h>

void matrix_display(Matrix m) {
    for (size_t i = 0; i < m.rows; i++) {
        for (size_t j = 0; j < m.cols; j++) {
            printf("%6.1f", *matrix_index(m, i, j));
        }
        printf("\n");
    }
}