#include "matrix.h"
#include <mpi.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "serial.h"

const int CHUNK_SIZE = 16;
const int CHUNK_ENTRIES = CHUNK_SIZE * CHUNK_SIZE;

typedef struct MatrixChunk {
    int row_start;
    int row_end;
    int col_start;
    int col_end;
} MatrixChunk;

MatrixChunk chunk_from_index(int i, int rows, int columns);

int main(int argc, char** argv) {
    int rank;
    int size;
    Matrix a;
    Matrix b;
    Matrix c;
    int d;
    int end[2] = { -1, -1 };

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int worker_count = size - 1;

    if (rank == 0) {
        d = 512;
        a = create_matrix(d, d, 7);
        b = create_matrix(d, d, 42);
        c = create_zeros_matrix(d, d);
        matrix_transpose(b);
    }

    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MatrixChunk* process_chunks;
    if (rank == 0) {
        process_chunks = malloc(worker_count * sizeof(MatrixChunk));
        for (int i = 0; i < worker_count; i++) {
            process_chunks[i] = chunk_from_index(i, d, d);
        }
    }

    int chunk_count = d * d / CHUNK_ENTRIES;

    if (rank == 0) {
        for (int i = 0; i < worker_count; i++) {
            MatrixChunk chunk = process_chunks[i];
            int chunk_size[2] = {
                chunk.row_end - chunk.row_start,
                chunk.col_end - chunk.col_start,
            };
            MPI_Send(chunk_size, 2, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(a.ptr + chunk.row_start * d, chunk_size[0] * d, MPI_FLOAT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(b.ptr + chunk.col_start * d, chunk_size[1] * d, MPI_FLOAT, i + 1, 0, MPI_COMM_WORLD);
        }

        for (int chunk_index = 0; chunk_index < chunk_count; chunk_index++) {
            float buffer[CHUNK_ENTRIES];
            
            MPI_Status status;
            MPI_Recv(buffer, CHUNK_ENTRIES, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
            int recv_rank = status.MPI_SOURCE;
            MatrixChunk prev_chunk = process_chunks[recv_rank - 1];
            int k = 0;
            for (int i = prev_chunk.row_start; i < prev_chunk.row_end; i++) {
                for (int j = prev_chunk.col_start; j < prev_chunk.col_end; j++) {
                    *matrix_index(c, i, j) = buffer[k];
                    k++;
                }
            }

            MatrixChunk next_chunk = chunk_from_index(chunk_index + worker_count, d, d);
            process_chunks[recv_rank - 1] = next_chunk;
            int chunk_size[2] = {
                next_chunk.row_end - next_chunk.row_start,
                next_chunk.col_end - next_chunk.col_start,
            };
            MPI_Send(chunk_size, 2, MPI_INT, recv_rank, 0, MPI_COMM_WORLD);
            MPI_Send(a.ptr + next_chunk.row_start * d, chunk_size[0] * d, MPI_FLOAT, recv_rank, 0, MPI_COMM_WORLD);
            MPI_Send(b.ptr + next_chunk.col_start * d, chunk_size[1] * d, MPI_FLOAT, recv_rank, 0, MPI_COMM_WORLD);
        }

        for (int i = 0; i < worker_count; i++) {
            MPI_Send(end, 2, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
        }

    } else {
        while (true) {
            int chunk_size[2];
            MPI_Recv(chunk_size, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            if (chunk_size[0] == -1) {
                printf("Worker %d done %d %d\n", rank, chunk_size[0], chunk_size[0]);
                break;
            }

            a = create_zeros_matrix(chunk_size[0], d);
            MPI_Recv(a.ptr, chunk_size[0] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            b = create_zeros_matrix(chunk_size[1], d);
            MPI_Recv(b.ptr, chunk_size[1] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            c = create_zeros_matrix(chunk_size[0], chunk_size[1]);

            matrix_multiply_transposed_serial(a, b, c);

            MPI_Send(c.ptr, c.rows * c.cols, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

            free_matrix(a);
            free_matrix(b);
            free_matrix(c);
        }
    }

    if (rank == 0) {
        free(process_chunks);
        free_matrix(a);
        free_matrix(b);
        free_matrix(c);
    }
    
    MPI_Finalize();
}

MatrixChunk chunk_from_index(int i, int rows, int columns) {
    int row = i * CHUNK_SIZE / rows * CHUNK_SIZE;
    int col = i * CHUNK_SIZE % rows;
    return (MatrixChunk) {
        .row_start = row,
        .row_end = row + CHUNK_SIZE,
        .col_start = col,
        .col_end = col + CHUNK_SIZE
    };
}