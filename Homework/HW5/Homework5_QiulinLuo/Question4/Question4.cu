#include <cuda.h>
#include <curand_kernel.h>
#include <iomanip>
#include <iostream>
#include <limits>
#include <stdio.h>

using namespace std;

const long long int avg_threads = 16; 
const long long int blocks_number = 640;
const long long int iterations_number = 100000000; 

__global__ void leibniz_method_gpu(long long int *series_add){
	__shared__ long long int compute_arr[avg_threads];

	int series = threadIdx.x + blockIdx.x * blockDim.x;
	curandState_t cur_state;
	curand_init(clock64(), series, 0, &cur_state);
	compute_arr[threadIdx.x] = 0;

	for (int i = 0; i < iterations_number; i++){
		float xt = curand_uniform(&cur_state); 
		float yt = curand_uniform(&cur_state); 
		float tmp = xt * xt + yt * yt;
		compute_arr[threadIdx.x] = compute_arr[threadIdx.x] - int(tmp) + 1; 
	}
	if (threadIdx.x == 0){
		series_add[blockIdx.x] = 0;
		for (int i = 0; i < avg_threads; i++) {
			series_add[blockIdx.x] = series_add[blockIdx.x] + compute_arr[i];
		}
	}
}

int main(){	
	long long int *final_result;
	long long int *tmp_result;
	final_result = new long long int[blocks_number]; 
	cudaMalloc(&tmp_result, sizeof(long long int) * blocks_number); 
	leibniz_method_gpu<<<blocks_number, avg_threads>>>(tmp_result);
	cudaMemcpy(final_result, tmp_result, sizeof(long long int) * blocks_number, cudaMemcpyDeviceToHost);
	cudaFree(tmp_result);

	long long int series_final_add = 0;
	for (int k = 0; k < blocks_number; k++) {
		series_final_add = series_final_add + final_result[k];
	}
	long long int tests_number = blocks_number * iterations_number * avg_threads;
	cout << "Test number:" << tests_number << endl;
	printf("Results: %.16f\n", 4.0 * (float)series_final_add / (float)tests_number);
	return 0;
}
