#pragma once

#include "matrix.h"

void matmul(Matrix a, Matrix b, Matrix c);

void matmul_transposed(Matrix a, Matrix bT, Matrix c);

void matmul_chunks2(Matrix a, Matrix b, Matrix c);