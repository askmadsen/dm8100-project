#pragma once

#include "matrix.h"

void split_matrices(Matrix a, Matrix b);
void receive_matrices(Matrix* a, Matrix* b, int* row, int* col);
void send_matrices(Matrix c, int row, int col);
void aggregate_matrices(Matrix c);