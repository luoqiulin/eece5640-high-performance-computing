#include <stdio.h>
#include <stdlib.h>
#include <cblas.h>
#include <time.h>
#define N 512        
#define LOOPS 10    

float a[N][N]; 
float b[N][N]; 
float c[N][N]; 
float a2[N][N]; 
float b2[N][N]; 
float c2[N][N]; 
float rowa[N * N];
float rowb[N * N];
float rowc[N * N];
float rowa2[N * N];
float rowb2[N * N];
float rowc2[N * N];

double CLOCK() {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC,  &t);
        return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

int main()
{
  
  int i,j,k,l, num_zeros;
  double start, finish, total;

  for(i=0; i<N; i++){    
    for(j=0; j<N; j++){
      a[i][j] = (float)(i+j);
      b[i][j] = (float)(i-j);
    }
  }
  
  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++){
      rowa[i * N + j] = a[i][j];
      rowb[i * N + j] = b[i][j];
    }
  }

  printf("starting dense matrix multiply \n");
  start = CLOCK();
  for (l=0; l<LOOPS; l++) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1, rowa, N, rowb, N, 0, rowc, N);
   }
finish = CLOCK();
total = finish-start;
printf("a result %g \n", rowc[7 * N + 8]); 
printf("The total time for matrix multiplication with dense matrices = %f ms\n", total);

 
  num_zeros = 0; 
  for(i=0; i<N; i++){    
    for(j=0; j<N; j++){
      if ((i<j)&&(i%2>0))
        {
         a2[i][j] = (float)(i+j);
         b2[i][j] = (float)(i-j);
        }
      else
        { 
         num_zeros++;
         a2[i][j] = 0.0;
         b2[i][j] = 0.0;
        }
       
     }
  }

  for(i = 0; i < N; i++){
    for(j = 0; j < N; j++){
      rowa2[i * N + j] = a2[i][j];
      rowb2[i * N + j] = b2[i][j];
    }
  }

  printf("starting sparse matrix multiply \n");
  start = CLOCK();
  for (l=0; l<LOOPS; l++) {
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1, rowa2, N, rowb2, N, 0, rowc2, N);
  }
finish = CLOCK();
total = finish-start;
printf("A result %g \n", rowc2[7 * N + 8]); 
printf("The total time for matrix multiplication with sparse matrices = %f ms\n", total);
printf("The sparsity of the a and b matrices = %f \n", (float)num_zeros/(float)(N*N));

return 0;
}

