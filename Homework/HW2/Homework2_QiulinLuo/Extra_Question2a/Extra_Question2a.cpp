#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <iostream>

using namespace std;

int threads_number;            
long long int darts_scope;
double sum = 0.0;
sem_t sem;

void *add_darts(void *spilt){
   long long i;
   long rank = (long)spilt;
   double value;
   double this_thread_count = 0.0;
 
   long long spilt_darts = darts_scope / threads_number;
   long long first_i = spilt_darts * rank;
   long long last_i = first_i + spilt_darts;
   value = first_i % 2 == 0? 1.0:-1.0;
   for(i = first_i; i < last_i; i++, value = -value){
      this_thread_count += value / (2 * i + 1);
   }
   sem_wait(&sem);
   sum += this_thread_count;
   sem_post(&sem);
   return NULL;
}

int main(void){
   clock_t initial, end;
   double total_time;
   cout << "number of darts:" << endl;
   cin >> darts_scope ;
   cout << "number of threads:" << endl;
   cin >> threads_number ;

   long count, count2;
   pthread_t tid[threads_number];
   
   initial = clock();

   sem_init(&sem, 0, 1);

   for(count = 0; count < threads_number; count++){ 
      pthread_create(&tid[count], NULL, add_darts, (void*)count);
   }

   for(count2 = 0; count2 < threads_number; count2++){ 
      pthread_join(tid[count2], NULL);
   }
   end = clock();
   total_time = (double)(end - initial) / CLOCKS_PER_SEC;
   printf("total time is: %fs\n", total_time);
   printf("%.20lf\n", 4 * sum);
   sem_destroy(&sem); 

   return 0;
}