#include "matrix.h"
#include <mpi.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

#include "serial.h"
#include "arg_parser.h"
#include "openmpi.h"

int parse_args(int argc, char** argv, int* d, char** dest) {
    ARG_INIT()
    INT_ARG(d)

    OPT_ARGS(
        OPT_STR_ARG("--dest", dest)
    )

    return 0;
}

int main_worker(void) {
    Matrix a;
    Matrix b;
    int row;
    int col;

    receive_matrices(&a, &b, &row, &col);

    Matrix c = matrix_calloc(a.rows, b.cols);
    matmul_chunks(a, b, c);

    send_matrices(c, row, col);

    return 0;
}

int main_orchestrator(int argc, char** argv) {
    int d;
    char* dest = NULL;
    parse_args(argc, argv, &d, &dest);
    
    int size;
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    Matrix a = matrix_filled(d, d, 7);
    Matrix b = matrix_filled(d, d, 13);
    Matrix c = matrix_calloc(d, d);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    split_matrices(a, b);
    main_worker();
    aggregate_matrices(c);

    clock_gettime(CLOCK_MONOTONIC, &end);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
    printf("%.6f\n", elapsed);

    if (dest != NULL) {
        matrix_save(c, dest);
    }

    matrix_free(a);
    matrix_free(b);
    matrix_free(c);

    return 0;
}

int main(int argc, char** argv) {
    MPI_Init(&argc, &argv);

    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    int error_code;
    if (rank == 0) {
        error_code = main_orchestrator(argc, argv);
    } else {
        error_code = main_worker();
    }

    MPI_Finalize();

    return error_code;
}
