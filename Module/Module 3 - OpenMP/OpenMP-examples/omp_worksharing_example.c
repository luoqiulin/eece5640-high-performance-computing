#include <stdlib.h>
#include <stdio.h>
#include <omp.h>
#define CHUNKSIZE  10
#define N       100

int main (int argc, char *argv[]) 
{
  int nthreads, tid, i, chunk;
  float a[N], b[N], c[N];

/* Some initializations */
for (i=0; i < N; i++)
  a[i] = b[i] = i * 1.0;
chunk = CHUNKSIZE;

#pragma omp parallel shared(a,b,c, nthreads,chunk) private(i,tid)
  {
  tid = omp_get_thread_num();
  if (tid == 0)
    {
    nthreads = omp_get_num_threads();
    printf("Number of threads = %d\n", nthreads);
    }
  printf("Thread %d starting...\n",tid);

  #pragma omp for schedule(static, chunk)
  for (i=0; i < N; i++)
    {
      c[i] = a[i] + b[i];
      usleep(1);
      printf("Thread %d: c[%d]= %f\n",tid,i,c[i]);
    }
  } /* end of parallel section */
}
