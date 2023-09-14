#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

int main (int argc, char *argv[]) 
{
int nthreads, tid, procs, maxt, inpar, dynamic, nested;

/* Start parallel region */
#pragma omp parallel private(nthreads, tid)
  {

  /* Obtain thread number */
  tid = omp_get_thread_num();

  /* Only master thread does this */
  if (tid == 0) 
    {
    printf("Thread %d getting environment info...\n", tid);

    /* Get environment information */
    procs = omp_get_num_procs(); /* Returns no. of cores available when called */

    nthreads = omp_get_num_threads(); /* Returns no. of threads in the current team */
    maxt = omp_get_max_threads(); /* Returns an upper bound on the no. of threads that cold be used to form a new team */
    inpar = omp_in_parallel(); /* Returns true if the current task is enclosed iby the active parallel region */
    dynamic = omp_get_dynamic(); /* Returns true if dynamic adjustment of the no. of threads is enabled */
    nested = omp_get_nested(); /* Returns true if nested parallelism is enabled */

    /* Print environment information */
    printf("Number of processors = %d\n", procs);
    printf("Number of threads = %d\n", nthreads);
    printf("Max threads = %d\n", maxt);
    printf("In parallel? = %d\n", inpar);
    printf("Dynamic threads enabled? = %d\n", dynamic);
    printf("Nested parallelism supported? = %d\n", nested);

    }

  }
}
