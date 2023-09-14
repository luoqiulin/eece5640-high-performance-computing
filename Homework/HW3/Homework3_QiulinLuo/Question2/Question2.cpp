#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#define N 512        
#define LOOPS 10   
#define threads_number 8

double a[N][N]; 
double b[N][N]; 
double c[N][N];
double a2[N][N]; 
double b2[N][N]; 
double c2[N][N];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex_2 = PTHREAD_MUTEX_INITIALIZER;

struct info {
    int begin;
    int stop;
};

void *matrix_multiplication1(void *t);
void *matrix_multiplication2(void *t);

void *matrix_multiplication1(void *t){
    struct info *data = (struct info *) t;
    for (int p = 0; p < N; p++){
        for (int q = 0; q < N; q++){
            long tmp = 0;
            for (int r = data->begin; r < data->stop; r++){
                tmp += a[p][r] * b[r][q];
            }
            pthread_mutex_lock(&mutex);
            c[p][q] += tmp;
            pthread_mutex_unlock(&mutex);
        }
    }
}

void *matrix_multiplication2(void *t){
    struct info *data = (struct info *) t;
    for (int p = 0; p < N; p++){
        for (int q = 0; q < N; q++){
            long tmp = 0;
            for (int r = data->begin; r < data->stop; r++){
                tmp += a2[p][r] * b2[r][q];
            }
            pthread_mutex_lock(&mutex_2);
            c2[p][q] += tmp;
            pthread_mutex_unlock(&mutex_2);
        }
    }
}

double CLOCK() {
        struct timespec t;
        clock_gettime(CLOCK_MONOTONIC,  &t);
        return (t.tv_sec * 1000)+(t.tv_nsec*1e-6);
}

int main()
{
  int i,j,k,l, num_zeros;
  double start, finish, total;
  pthread_t threads[threads_number];
  pthread_t threads2[threads_number];
  struct info sub[threads_number];
  int c_begin = 0;
  int partition = N / threads_number;
  for (int f = 0; f < threads_number; f++){
      sub[f].begin = c_begin;
      sub[f].stop = c_begin + partition;
      c_begin += partition;
  }
  sub[threads_number - 1].stop = N;

  for(i=0; i<N; i++){    
    for(j=0; j<N; j++){
      a[i][j] = (double)(i+j);
      b[i][j] = (double)(i-j);
    }
  }
  
  printf("starting dense matrix multiply \n");
  start = CLOCK();
  for (l=0; l<LOOPS; l++) {
      for(int g = 0; g < threads_number; g++){
          pthread_create(&threads[g], NULL, matrix_multiplication1, &sub[g]);
      }
      for(int g = 0; g < threads_number; g++){
          pthread_join(threads[g], NULL);
      }
   }
finish = CLOCK();
total = finish-start;
printf("a result %g \n", c[7][8] / 10); 
printf("The total time for matrix multiplication with dense matrices = %f ms\n", total);

  num_zeros = 0; 
  for(i=0; i<N; i++){    
    for(j=0; j<N; j++){
      if ((i<j)&&(i%2>0))
        {
         a2[i][j] = (double)(i+j);
         b2[i][j] = (double)(i-j);
        }
      else
        { 
         num_zeros++;
         a2[i][j] = 0.0;
         b2[i][j] = 0.0;
        }
       
     }
  }

  struct info sub2[threads_number];
  int c_begin2 = 0;
  int partition2 = N / threads_number;
  for (int f = 0; f < threads_number; f++){
      sub2[f].begin = c_begin2;
      sub2[f].stop = c_begin2 + partition2;
      c_begin2 += partition2;
  }
  sub2[threads_number - 1].stop = N;

  printf("starting sparse matrix multiply \n");
  start = CLOCK();
  for (l=0; l<LOOPS; l++) {
      for(int g = 0; g < threads_number; g++){
          pthread_create(&threads2[g], NULL, matrix_multiplication2, &sub2[g]);
      }
      for(int g = 0; g < threads_number; g++){
          pthread_join(threads2[g], NULL);
      }
   }
finish = CLOCK();
total = finish-start;
printf("A result %g \n", c2[7][8] / 10); 
printf("The total time for matrix multiplication with sparse matrices = %f ms\n", total);
printf("The sparsity of the a and b matrices = %f \n", (float)num_zeros/(float)(N*N));

return 0;
}

