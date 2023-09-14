#include <cuda.h>
#include <curand_kernel.h>
#include <omp.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

const int bins_number = 10;
const int blocks_number = 512;
const int blocks_average_threads = 64;
const int omp_threads_number = 64;
int threads_total_number = blocks_number * blocks_average_threads;

__host__ __device__ int bins_verify(int element, float* mopr, float* Mopr){
    for(int k = 0; k < bins_number; k++) 
    {
        if(element >= mopr[k] && element < Mopr[k]){
            return k;
        }       
    }
    return -1;
}

__global__ void create_hist(int* value, int* bin_init, int* belement, float* mopr, float* Mopr, int* mtid, int* Mtid){   
    int __shared__ average_hists[blocks_average_threads][bins_number];
    int sqe_number;
    for (int i = 0; i < bins_number; i = i + 1){
        average_hists[threadIdx.x][i] = 0;
    }  

    __syncthreads();

    for(int j = mtid[blockIdx.x*blockDim.x + threadIdx.x]; j < Mtid[blockIdx.x*blockDim.x + threadIdx.x]; j++){
        sqe_number = bins_verify(value[j], mopr, Mopr);    
        average_hists[threadIdx.x][sqe_number] = average_hists[threadIdx.x][sqe_number] + 1;
        belement[sqe_number] = value[j];
    }

    __syncthreads();
    
    if (threadIdx.x == 0){
        for (int tmp1 = 0; tmp1 < bins_number; tmp1 = tmp1 + 1){   
            for (int tmp2 = 0; tmp2 < blocks_average_threads; tmp2 = tmp2 + 1) {   
                bin_init[blockIdx.x * bins_number + tmp1] += average_hists[tmp2][tmp1];
            }
        }
    }
}

__global__ void bins_increment(int* bin_init, int* bin_init_aft){   
    for (int tmp1 = 0; tmp1 < bins_number; tmp1 = tmp1 + 1){   
        for (int tmp2 = 0; tmp2 < blocks_number; tmp2 = tmp2 + 1){   
            bin_init_aft[tmp1] = bin_init_aft[tmp1] + bin_init[tmp2 * bins_number + tmp1];
        }
    }
}

void value_initialization(int* value, int value_extent){
    for(int i = 0; i < value_extent; i++){
        value[i] = rand() % 10000000 + 1;
    }
}

void value_reference(int* mtid, int* Mtid, int value_extent){
    float blank = floor((float)value_extent / (float)threads_total_number);
    for(int i = 0; i < threads_total_number; i++){
        mtid[i] = i * blank;
        Mtid[i] = (i + 1) * blank;
        if(i == threads_total_number - 1){
            Mtid[i] = value_extent;
        }
    }
}

void bins_installing(float* mopr, float* Mopr, int bins_number){
    float blank = ceil((float)(10000000 - 1) / (float)bins_number);
    for(int i = 0; i < bins_number; i++){
        mopr[i] = i * blank + 1;
        if(i == bins_number - 1){
            Mopr[i] = 10000001;
        }else{
            Mopr[i] = (i + 1) * blank + 1;
        }
    }
}

void hist_output(int* hist_value){
    int count = 0;
    for(int i = 0; i < bins_number; i++){
        count += hist_value[i];
        printf("number of values in bins %d: %d\n", i, hist_value[i]);
    }
}

void cuda_version(int* value, int value_extent, float* mopr, float* Mopr){   
    srand(time(NULL));
    struct timespec initial_1, finish_1, initial_2, finish_2;
    int* value_gpu;
    int* mtid, * Mtid;
    int* cuda_mtid, * cuda_Mtid;
    float* cuda_mopr, * cuda_Mopr;
    int* cuda_bin_init;
    int* bin_init_aft, * cuda_bin_init_aft;
    int* belement, * cuda_belement;

    clock_gettime(CLOCK_MONOTONIC, &initial_1);

    cudaMalloc((void**) &cuda_mopr, sizeof(float)*bins_number);
    cudaMalloc((void**) &cuda_Mopr, sizeof(float)*bins_number);
    mtid = (int*)calloc(threads_total_number, sizeof(int));
    Mtid = (int*)calloc(threads_total_number, sizeof(int));
    cudaMalloc((void**) &cuda_mtid, sizeof(int)*threads_total_number);
    cudaMalloc((void**) &cuda_Mtid, sizeof(int)*threads_total_number);
    cudaMalloc((void**) &cuda_bin_init, sizeof(int)*bins_number*blocks_number);
    cudaMemset(cuda_bin_init, 0, sizeof(int)*bins_number*blocks_number);
    bin_init_aft = (int*)calloc(bins_number, sizeof(int));
    cudaMalloc((void**) &cuda_bin_init_aft, sizeof(int)*bins_number);
    cudaMemset(bin_init_aft, 0, sizeof(int)*bins_number);
    belement = (int*)calloc(bins_number, sizeof(int));
    cudaMalloc((void**) &cuda_belement, sizeof(int)*bins_number);
    value_reference(mtid, Mtid, value_extent);
    cudaMalloc((void**) &value_gpu, sizeof(int)*value_extent);
    cudaMemcpy(value_gpu, value, sizeof(int)*value_extent, cudaMemcpyHostToDevice);   
    cudaMemcpy(cuda_mopr, mopr, sizeof(int)*bins_number, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_Mopr, Mopr, sizeof(int)*bins_number, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_mtid, mtid, sizeof(int)*threads_total_number, cudaMemcpyHostToDevice);
    cudaMemcpy(cuda_Mtid, Mtid, sizeof(int)*threads_total_number, cudaMemcpyHostToDevice);

    clock_gettime(CLOCK_MONOTONIC, &initial_2);
    create_hist<<<blocks_number, blocks_average_threads>>>(value_gpu, cuda_bin_init, cuda_belement, cuda_mopr, cuda_mopr, cuda_mtid, cuda_mtid);
    bins_increment<<<1, 1>>>(cuda_bin_init, cuda_bin_init_aft);
    clock_gettime(CLOCK_MONOTONIC, &finish_2);

    double duration_2 = (finish_2.tv_sec - initial_2.tv_sec);
    duration_2 += (finish_2.tv_nsec - initial_2.tv_nsec) / 1000000000.0;

    cudaMemcpy(bin_init_aft, cuda_bin_init_aft, sizeof(int)*bins_number, cudaMemcpyDeviceToHost);
    cudaMemcpy(belement, cuda_belement, sizeof(int)*bins_number, cudaMemcpyDeviceToHost);

    clock_gettime(CLOCK_MONOTONIC, &finish_1);
    double duration_1 = (finish_1.tv_sec - initial_1.tv_sec);
    duration_1 += (finish_1.tv_nsec - initial_1.tv_nsec) / 1000000000.0;

    printf("GPU Time: %f\n", duration_1);
    //hist_output(bin_init_aft);
    cudaFree(value_gpu); 
    cudaFree(cuda_mopr);
    cudaFree(cuda_Mopr);
    cudaFree(cuda_mtid);
    cudaFree(cuda_Mtid);
    cudaFree(cuda_bin_init);
    cudaFree(cuda_bin_init_aft);
    cudaFree(cuda_belement);
}

void cpu_version(int* value, int value_extent, float* mopr, float* Mopr){
    struct timespec initial, finish;
    clock_gettime(CLOCK_MONOTONIC, &initial);
    omp_set_num_threads(omp_threads_number);
    int i, j;
    int sequence_threads, sequence_bins, threads_number;
    int* bin_init, *bin_init_aft;
    bin_init = (int *)calloc(omp_threads_number * bins_number, sizeof(int));
    bin_init_aft = (int *)calloc(bins_number, sizeof(int));

    #pragma omp parallel for private(i, sequence_threads)
    for(i = 0; i < value_extent; i++){
        sequence_threads = omp_get_num_threads();
        if(sequence_threads == 0){
            threads_number = omp_get_num_threads();
        }
        sequence_bins = bins_verify(value[i], mopr, Mopr);
        assert(("class index should not be negative", sequence_bins >= 0));
        #pragma omp atomic
        bin_init[sequence_threads*bins_number + sequence_bins] += 1;
    }
    int tmp = 0;
    #pragma omp parallel for schedule(static) private(i, j) reduction(+:tmp)
    for(i = 0; i < bins_number; i++){
        tmp = 0;
        for(j = 0; j < omp_threads_number; j++){
            tmp += bin_init[j * bins_number + i];
        }
        bin_init_aft[i] = tmp;
    }
    clock_gettime(CLOCK_MONOTONIC, &finish);
    double duration = (finish.tv_sec - initial.tv_sec);
    duration += (finish.tv_nsec - initial.tv_nsec) / 1000000000.0;
    printf("CPU Time: %f\n", duration);
    //hist_output(bin_init_aft);
}

int main(int argc, char *argv[]){
    assert(("./Question1 <number of values>", argc == 2));
    int value_extent = atoi(argv[1]);
    int * value; 
    float * mopr, * Mopr;
    value = (int *)calloc(value_extent, sizeof(int));
    mopr = (float *)calloc(bins_number, sizeof(float));
    Mopr = (float *)calloc(bins_number, sizeof(float));

    value_initialization(value, value_extent);
    bins_installing(mopr, Mopr, bins_number);
    cuda_version(value, value_extent, mopr, Mopr);
    cpu_version(value, value_extent, mopr, Mopr);
    
    return 0;
}