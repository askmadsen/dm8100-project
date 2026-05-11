#include "matrix.h"

#include <mpi.h>
#include <stdlib.h>

typedef struct MatrixChunk {
    int row;
    int rows;
    int col;
    int cols;
} MatrixChunk;

MatrixChunk up_chunk(MatrixChunk m) {
    return (MatrixChunk) { 
        .row = m.row, 
        .rows = m.rows / 2,
        .col = m.col,
        .cols = m.cols
    };
}

MatrixChunk down_chunk(MatrixChunk m) {
    return (MatrixChunk) { 
        .row = m.row + m.rows / 2, 
        .rows = m.rows - m.rows / 2,
        .col = m.col,
        .cols = m.cols
    };
}

MatrixChunk left_chunk(MatrixChunk m) {
    return (MatrixChunk) { 
        .row = m.row, 
        .rows = m.rows,
        .col = m.col,
        .cols = m.cols / 2
    };
}

MatrixChunk right_chunk(MatrixChunk m) {
    return (MatrixChunk) { 
        .row = m.row, 
        .rows = m.rows,
        .col = m.col + m.cols / 2,
        .cols = m.cols - m.cols / 2
    };
}

void split_matrices(Matrix a, Matrix b) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MatrixChunk* chunks = malloc(size * sizeof(MatrixChunk));

    chunks[0] = (MatrixChunk) { 
        .row = 0, 
        .rows = a.rows,
        .col = 0,
        .cols = b.cols,
    };

    int chunks_placed = 1;

    for (int iteration = 0; chunks_placed < size; iteration++) {
        int offset = chunks_placed;
        for (int i = 0; i < offset && chunks_placed < size; i++) {
            if (iteration % 2 == 0) {
                chunks[i + offset] = down_chunk(chunks[i]);
                chunks[i] = up_chunk(chunks[i]);
            } else {
                chunks[i + offset] = right_chunk(chunks[i]);
                chunks[i] = left_chunk(chunks[i]);
            }
            chunks_placed++;
        }
    }

    for (int rank = 0; rank < size; rank++) {
        MatrixChunk m = chunks[rank];
        printf("%d: %d %d %d %d\n", rank, m.row, m.rows, m.col, m.cols);
        float* a_buffer = a.ptr + m.row * a.rows;
        float* b_buffer = malloc(b.rows * m.cols * sizeof(float));

        for (int i = 0; i < b.rows * m.cols; i++) {
            b_buffer[i] = *matrix_index(b, i / m.cols, m.col + i % m.cols);
        }

        MPI_Request request;

        MPI_Isend(&m.row, 1, MPI_INT, rank, 0, MPI_COMM_WORLD, &request);
        MPI_Isend(&m.rows, 1, MPI_INT, rank, 1, MPI_COMM_WORLD, &request);
        MPI_Isend(&m.col, 1, MPI_INT, rank, 2, MPI_COMM_WORLD, &request);
        MPI_Isend(&m.cols, 1, MPI_INT, rank, 3, MPI_COMM_WORLD, &request);
        MPI_Isend(&a.cols, 1, MPI_INT, rank, 4, MPI_COMM_WORLD, &request);
        MPI_Isend(&b.rows, 1, MPI_INT, rank, 5, MPI_COMM_WORLD, &request);
        MPI_Isend(a_buffer, m.rows * a.cols, MPI_FLOAT, rank, 6, MPI_COMM_WORLD, &request);
        MPI_Isend(b_buffer, b.rows * m.cols, MPI_FLOAT, rank, 7, MPI_COMM_WORLD, &request);

        // free(b_buffer);
    }
}

void receive_matrices(Matrix* a, Matrix* b, int* row, int* col) {
    int rows;
    int cols;
    int a_cols;
    int b_rows;
    MPI_Recv(row, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&rows, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(col, 1, MPI_INT, 0, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&cols, 1, MPI_INT, 0, 3, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&a_cols, 1, MPI_INT, 0, 4, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&b_rows, 1, MPI_INT, 0, 5, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    float* a_buffer = malloc(rows * a_cols * sizeof(float));
    float* b_buffer = malloc(b_rows * cols * sizeof(float));
    MPI_Recv(a_buffer, rows * a_cols, MPI_FLOAT, 0, 6, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(b_buffer, b_rows * cols, MPI_FLOAT, 0, 7, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    *a = matrix_new(a_buffer, rows, a_cols);
    *b = matrix_new(b_buffer, b_rows, cols);
}

void send_matrices(Matrix c, int row, int col) {
    MPI_Request request;
    MPI_Isend(&row, 1, MPI_INT, 0, 8, MPI_COMM_WORLD, &request);
    MPI_Isend(&c.rows, 1, MPI_INT, 0, 9, MPI_COMM_WORLD, &request);
    MPI_Isend(&col, 1, MPI_INT, 0, 10, MPI_COMM_WORLD, &request);
    MPI_Isend(&c.cols, 1, MPI_INT, 0, 11, MPI_COMM_WORLD, &request);
    MPI_Isend(c.ptr, c.rows * c.cols, MPI_FLOAT, 0, 12, MPI_COMM_WORLD, &request);
}

void aggregate_matrices(Matrix c) {
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int rank = 0; rank < size; rank++) {
        int row;
        int rows;
        int col;
        int cols;
        float* data = malloc(rows * cols * sizeof(float));
        
        MPI_Recv(&row, 1, MPI_INT, rank, 8, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&rows, 1, MPI_INT, rank, 9, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&col, 1, MPI_INT, rank, 10, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(&cols, 1, MPI_INT, rank, 11, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(data, rows * cols, MPI_FLOAT, rank, 12, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        for (int i = row; i < row + rows; i++) {
            for (int j = col; j < col + cols; j++) {
                *matrix_index(c, i, j) = data[i * rows + j];
            }
        }

        free(data);
    }
}