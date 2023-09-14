#include <cmath>
#include <mpi.h>
#include <stdlib.h>
#include <chrono>
#include <iostream>

#define BIN_NUM 100
#define DATA_SET 2000000
#define INT_SCOPE 1000000

using namespace std;
using namespace std::chrono;

int main(int argc, char* argv[]){
	int DATA_VALUES[DATA_SET];
	for(int i = 0; i < DATA_SET; i++){
		DATA_VALUES[i] = (rand() % INT_SCOPE) + 1;
	}
		
	auto initial_time = high_resolution_clock::now();

	MPI_Init(&argc, &argv);

    int mpi_rank, mpi_size;
	int bins_buffer[BIN_NUM];
	for(int k = 0; k < BIN_NUM; k++){
		bins_buffer[k] = 0;
	}

	MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);

	int scope_partition = floor(DATA_SET / mpi_size);
	int bin_partition = floor(INT_SCOPE / BIN_NUM);
	int single_bin;
	int multiple_bin[BIN_NUM];

	for(int i = (mpi_rank) * (scope_partition); i < ((mpi_rank + 1) * (scope_partition)) - 1; i++){
		single_bin = floor(DATA_VALUES[i] / bin_partition);
		bins_buffer[single_bin] = bins_buffer[single_bin] + 1;
	}

	for(int k = 0; k < BIN_NUM; k++){
		multiple_bin[k] = 0;
	}
	for(int i = 0; i < BIN_NUM; i++){
		MPI_Reduce(&bins_buffer[i], &multiple_bin[i], 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
	}
		
	if(mpi_rank == 0){
        auto end_time = high_resolution_clock::now();
        auto cpu_running_time = duration_cast<microseconds>(end_time - initial_time);
		for (int j = 0; j < BIN_NUM; j++){
			cout << "bin's number: " << j + 1 << " =========== " << multiple_bin[j] << endl;
		}	
        cout << "cpu running time: " << cpu_running_time.count() << " microseconds" << endl;
	}

	MPI_Finalize();
    return 0;
}
