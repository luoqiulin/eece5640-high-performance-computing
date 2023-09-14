#include <cuda.h>
#include <curand_kernel.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

const int N = 64;
const int average_threads = 16;
const int blocks_number = 4;
const int opr_interspace = average_threads + 2;

__global__ void normal(float matrix_1[N][N][N], float matrix_2[N][N][N]){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    int j = blockIdx.y * blockDim.y + threadIdx.y;
    int k = blockIdx.z * blockDim.z + threadIdx.z;
    if (i > 0 && j > 0 && k > 0 && i < N - 1 && j < N - 1 && k < N - 1){
        int tmp1 = matrix_1[i - 1][j][k] + matrix_1[i + 1][j][k] + matrix_1[i][j - 1][k];
        int tmp2 = matrix_1[i][j + 1][k] + matrix_1[i][j][k - 1] + matrix_1[i][j][k + 1];
        matrix_2[i][j][k] = 0.8 * (tmp1 + tmp2);
    }
}

__global__ void specific(float matrix_1[N][N][N], float matrix_2[N][N][N]){   
    float __shared__ public_matrix[opr_interspace][opr_interspace][opr_interspace];
    int thx = threadIdx.x + 1;
    int thy = threadIdx.y + 1;
    int thz = threadIdx.z + 1;
    int i = blockIdx.x * blockDim.x + threadIdx.x + 1;
    int j = blockIdx.y * blockDim.y + threadIdx.y + 1;
    int k = blockIdx.z * blockDim.z + threadIdx.z + 1;
    for(int r = -1; r < 2; r += 1){
        for(int s = -1; s < 2; s += 1){
            for(int t = -1; t < 2; t += 1){
                if(i + r < N && j + s < N && k + t < N){
                    public_matrix[thx + r][thy + s][thz + t] = matrix_1[i + r][j + s][k + t];
                }
            }
        }
    }
    __syncthreads();
    if (i > 0 && j > 0 && k > 0 && i < N - 1 && j < N - 1 && k < N - 1){   
        int tmp1 = public_matrix[thx - 1][thy][thz] + public_matrix[thx + 1][thy][thz] + public_matrix[thx][thy - 1][thz];
        int tmp2 = public_matrix[thx][thy + 1][thz] + public_matrix[thx][thy][thz - 1] + public_matrix[thx][thy][thz + 1];
        matrix_2[i][j][k] = 0.8 * (tmp1 + tmp2);
    }
}

int main(int argc, char *argv[]){
    float * tiled_matrix_1, * non_tiled_matrix_1, * matrix_2; 
    cudaMallocManaged(&tiled_matrix_1, N * N * N * sizeof(float));
    cudaMallocManaged(&non_tiled_matrix_1, N * N * N * sizeof(float));
    cudaMallocManaged(&matrix_2, N * N * N * sizeof(float));
    float matrix_2_values[N][N][N];
    float matrix_1_values[N][N][N];
    memset(matrix_1_values, 0, N * N * N * sizeof(matrix_1_values[0][0][0]));
    for(int i = 0; i < N; i++){
        for(int j = 0; j < N; j++){
            for(int k = 0; k < N; k++){
                matrix_2_values[i][j][k] = rand() % 100 + 1;
                matrix_2_values[i][j][k] = (float)rand() / (float)(RAND_MAX) * matrix_2_values[i][j][k];
            }
        }
    }
    float (*tiled_matrix_1_values)[N][N] = reinterpret_cast<float (*)[N][N]>(tiled_matrix_1);
    float (*non_tiled_matrix_1_values)[N][N] = reinterpret_cast<float (*)[N][N]>(non_tiled_matrix_1);
    memcpy(matrix_2, &matrix_2_values[0][0][0], sizeof(matrix_2_values));
    dim3 avgThreads_blocks(average_threads, average_threads, average_threads);
    dim3 blocks(blocks_number, blocks_number, blocks_number);
    
    cudaEvent_t initial_time, end_time;
    cudaEvent_t initial_time_2, end_time_2;

    cudaEventCreate(&initial_time);
    cudaEventCreate(&end_time);
    cudaEventRecord(initial_time);
    specific<<<blocks, avgThreads_blocks>>>(reinterpret_cast<float (*)[N][N]>(matrix_2), tiled_matrix_1_values);
    cudaEventRecord(end_time);
    cudaEventSynchronize(end_time);
    float duration_1 = 0;
    cudaEventElapsedTime(&duration_1, initial_time, end_time);
    printf("non tiled method: %f milliseconds\n", duration_1);

    cudaEventCreate(&initial_time_2);
    cudaEventCreate(&end_time_2);
    cudaEventRecord(initial_time_2);
    normal<<<blocks, avgThreads_blocks>>>(reinterpret_cast<float (*)[N][N]>(matrix_2), non_tiled_matrix_1_values);
    cudaEventRecord(end_time_2);
    cudaEventSynchronize(end_time_2);
    float duration_2 = 0;
    cudaEventElapsedTime(&duration_2, initial_time_2, end_time_2);
    printf("tiled method: %f milliseconds\n", duration_2);
    cudaDeviceSynchronize();
    return 0;
}
