#include "matrix.h"
#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "serial.h"
#include "arg_parser.h"

const int END[2] = { -1, -1 };

typedef struct MatrixChunk {
    int row_start;
    int row_end;
    int col_start;
    int col_end;
} MatrixChunk;

MatrixChunk chunk_from_index(int i, int rows, int cols, int chunk_size);

int parse_args(int argc, char** argv, int* d, char** dest, int* chunk_size) {
    ARG_INIT()
    INT_ARG(d)

    OPT_ARGS(
        OPT_STR_ARG("--dest", dest)
        OPT_INT_ARG("--chunk-size", chunk_size)
    )

    return 0;
}

// Orchestrator process that divides the matrix multiplication work into chunks and distributes them to worker processes,
// then collects the results and assembles the final matrix.
int main_orchestrator(int argc, char** argv) {
    int d;
    char* dest = NULL;
    int chunk_size = 16;
    parse_args(argc, argv, &d, &dest, &chunk_size);
    int chunk_entries = chunk_size * chunk_size;
    int chunk_count = d * d / chunk_entries;

    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    int worker_count = size - 1;

    Matrix a = matrix_filled(d, d, 7);
    Matrix b = matrix_filled(d, d, 13);
    Matrix c = matrix_calloc(d, d);

    matrix_transpose(b);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);

    MatrixChunk* process_chunks = malloc(worker_count * sizeof(MatrixChunk));
    for (int i = 0; i < worker_count; i++) {
        process_chunks[i] = chunk_from_index(i, d, d, chunk_size);
    }

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
        float* buffer = malloc(chunk_entries * sizeof(float));

        MPI_Status status;
        MPI_Recv(buffer, chunk_entries, MPI_FLOAT, MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, &status);
        int recv_rank = status.MPI_SOURCE;
        MatrixChunk prev_chunk = process_chunks[recv_rank - 1];
        int k = 0;
        for (int i = prev_chunk.row_start; i < prev_chunk.row_end; i++) {
            for (int j = prev_chunk.col_start; j < prev_chunk.col_end; j++) {
                *matrix_index(c, i, j) = buffer[k];
                k++;
            }
        }

        if (chunk_index >= chunk_count - worker_count) {
            continue;
        }

        MatrixChunk next_chunk = chunk_from_index(chunk_index + worker_count, d, d, chunk_size);
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
        MPI_Send(END, 2, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    matrix_save(c, dest);

    MPI_Finalize();

    return 0;
}

// Worker process that receives chunks of the input matrices, performs the matrix multiplication for that chunk,
// and sends the result back to the orchestrator.
int main_worker(void) {
    int d;

    MPI_Bcast(&d, 1, MPI_INT, 0, MPI_COMM_WORLD);

    while (true) {
        int chunk_size[2];
        MPI_Recv(chunk_size, 2, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        if (chunk_size[0] == -1) {
            break;
        }

        Matrix a = matrix_calloc(chunk_size[0], d);
        Matrix b = matrix_calloc(chunk_size[1], d);
        Matrix c = matrix_calloc(chunk_size[0], chunk_size[1]);
        MPI_Recv(a.ptr, chunk_size[0] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(b.ptr, chunk_size[1] * d, MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

        matmul_transposed(a, b, c);

        MPI_Send(c.ptr, c.rows * c.cols, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);

        matrix_free(a);
        matrix_free(b);
        matrix_free(c);
    }

    MPI_Finalize();

    return 0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    if (rank == 0) {
        return main_orchestrator(argc, argv);
    } else {
        return main_worker();
    }
}

// Converts a linear chunk index into a MatrixChunk struct that defines the row and column range for that chunk,
// based on the total dimensions of the matrix and the desired chunk size.
MatrixChunk chunk_from_index(int i, int rows, int cols, int chunk_size) {
    int row = i * chunk_size / rows * chunk_size;
    int col = i * chunk_size % rows;
    int row_end = row + chunk_size < rows ? row + chunk_size : rows;
    int col_end = col + chunk_size < cols ? col + chunk_size : cols;
    return (MatrixChunk) {
        .row_start = row,
        .row_end = row_end,
        .col_start = col,
        .col_end = col_end
    };
}
