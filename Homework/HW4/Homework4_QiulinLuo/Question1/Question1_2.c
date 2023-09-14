#include <mpi.h>
#include <stdio.h>

int main (int argc, char** argv){

    MPI_Init(NULL, NULL);

    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char processor_name [MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    int number = 0;
    if (world_rank == 0){
        printf("Increase: The value is %d. The processor is %s. The process is %d\n", number, processor_name, world_rank);
        number++;
        MPI_Send(&number, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    }
    if (world_rank != 0){
        MPI_Recv(&number, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Increase: The value is %d. The processor is %s. The process is %d\n", number, processor_name, world_rank);
        number++;
        if(world_rank < world_size - 1){
            MPI_Send(&number, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD);
        }
    }
    if (world_rank == 31){
        printf("Decrease: The value is %d. The processor is %s. The process is %d\n", number, processor_name, world_rank);
        number--;
        MPI_Send(&number, 1, MPI_INT, 30, 0, MPI_COMM_WORLD);
    }
    if (world_rank != 31){
        MPI_Recv(&number, 1, MPI_INT, world_rank + 1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Decrease: The value is %d. The processor is %s. The process is %d\n", number, processor_name, world_rank);
        number--;
        if(world_rank > 0){
            MPI_Send(&number, 1, MPI_INT, world_rank - 1, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Finalize();
}