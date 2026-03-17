#include "matrix.h"
#include <mpi.h>
#include <stdio.h>

typedef struct MatrixChunk {
    size_t row_start;
    size_t row_end;
    size_t col_start;
    size_t col_end;
} MatrixChunk;

int main(int argc, char** argv) {
    int rank;
    int size;
    Matrix a;
    Matrix b;
    Matrix c;
    int d;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        d = 4;
        a = create_matrix(d, d, 7);
        b = create_matrix(d, d, 42);
        matrix_transpose(b);
        printf("A\n");
        matrix_display(a);
        printf("B\n");
        matrix_display(b);
    }

    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MatrixChunk process_chunks[4];

    process_chunks[0] = (MatrixChunk) {
        .row_start = 0,
        .row_end = 1,
        .col_start = 0,
        .col_end = 1,
    };

    process_chunks[1] = (MatrixChunk) {
        .row_start = 0,
        .row_end = 1,
        .col_start = 2,
        .col_end = 3,
    };

    process_chunks[2] = (MatrixChunk) {
        .row_start = 2,
        .row_end = 3,
        .col_start = 0,
        .col_end = 1,
    };

    process_chunks[3] = (MatrixChunk) {
        .row_start = 2,
        .row_end = 3,
        .col_start = 2,
        .col_end = 3,
    };

    if (rank == 0) {
        for (int i = 0; i < 4; i++) {
            MatrixChunk chunk = process_chunks[i];
            int chunk_size[2] = {
                chunk.row_end - chunk.row_start + 1,
                chunk.col_end - chunk.col_start + 1,
            };
            MPI_Send(chunk_size, 2, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(a.ptr + chunk.row_start * d, chunk_size[0] * d, MPI_FLOAT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(b.ptr + chunk.col_start * d, chunk_size[1] * d, MPI_FLOAT, i + 1, 0, MPI_COMM_WORLD);
        }


    } else {
        int chunk_size[2];
        MPI_Recv(chunk_size, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        Matrix a = create_zeros_matrix(chunk_size[0], d);
        MPI_Recv(a.ptr, chunk_size[0] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        Matrix b = create_zeros_matrix(chunk_size[1], d);
        MPI_Recv(b.ptr, chunk_size[1] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        printf("START %d\n", rank);
        printf("A\n");
        matrix_display(a);
        printf("B\n");
        matrix_display(b);
        printf("END %d\n", rank);
    }
    
    MPI_Finalize();
}