#include <mpi.h>
#include <stdio.h>

int main(int argc, char** argv)
{
    int rank, value, size;
    value = 0;
    MPI_Status status;
    char name[MPI_MAX_PROCESSOR_NAME];
    int len;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPI_Get_processor_name(name,&len);
    if (rank == 0)
    {
        fprintf(stderr, "process %d read value %d\n", rank, value);
        fflush(stderr);
        if (size > 1)
        {
            value++;
            MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            fprintf(stderr, "process %d send value %d to process %d\n", rank, value, rank + 1);
            fflush(stderr);
        }
    }
    else
    {
        MPI_Recv(&value, 1, MPI_INT, rank - 1, 0, MPI_COMM_WORLD, &status);
        fprintf(stderr, "process %d receive value %d from %d\n", rank, value, rank - 1);
        fflush(stderr);
        if (rank < size - 1)
        {
            value++;
            MPI_Send(&value, 1, MPI_INT, rank + 1, 0, MPI_COMM_WORLD);
            fprintf(stderr, "process %d send value %d to process %d\n", rank, value, rank + 1);
            fflush(stderr);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
  
    MPI_Finalize();
    return 0;
}