#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv) {
    int rank;
    int size;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    for (int i = 0; i < 10000; i++) {
        if (rank == 0) {
            MPI_Send(&i, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
        } else {
            int message;
            MPI_Recv(&message, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            printf("%d\n", message);
        }
    }

    MPI_Finalize();
}