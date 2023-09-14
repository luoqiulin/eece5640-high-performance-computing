#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>
#include <iostream>

using namespace std;

int main(void){
   
   long long int this_thread_count = 0;
   long long int darts_scope;
   int threads_number;
   double edge1, edge2;
   long long int i;
   clock_t initial, end;
   double total_time;

   cout << "number of darts:" << endl;
   cin >> darts_scope ;
   cout << "number of threads:" << endl;
   cin >> threads_number ;

   srand(time(NULL));
   
   initial = clock();
   #pragma omp parallel for num_threads(threads_number) default(none) \
      reduction(+:this_thread_count) shared(darts_scope) private(i, edge1, edge2)
   for(i = 0; i < darts_scope; i++){
      edge1 = (double)rand() / (double)RAND_MAX;
      edge2 = (double)rand() / (double)RAND_MAX;
      if(edge1 * edge1 + edge2 * edge2 <= 1){
         this_thread_count++;
      }
   }
   end = clock();
   total_time = (double)(end - initial) / CLOCKS_PER_SEC;
   double final_pi = (double)this_thread_count / darts_scope * 4;
   
   printf("%.20lf\n", final_pi);
   printf("total time is: %fs\n", total_time);

   return 0;
}