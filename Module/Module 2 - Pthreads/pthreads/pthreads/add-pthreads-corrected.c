#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <iostream>
#include <pthread.h>

using namespace std;

int darts_scope;
int threads_number;
double final_pi = 0.0;
pthread_mutex_t mutex;

void* add_darts(void* spilt)
{
   int i;
   int rank = *(int *) spilt;
   double this_thread_count = 0.0;
   int spilt_darts = darts_scope/threads_number;
   int first_i = spilt_darts * rank;
   int last_i = first_i + spilt_darts;
    
   for(i = first_i; i < last_i; i++)
   {
      srand(i);
      double edge_1 = ((double)(rand() % darts_scope)) / darts_scope;
      double edge_2 = ((double)(rand() % darts_scope)) / darts_scope;
      if(edge_1 * edge_1 + edge_2 * edge_2 <= 1)
         this_thread_count++;
   }
    
   pthread_mutex_lock(&mutex);
   final_pi += this_thread_count;
   pthread_mutex_unlock(&mutex);
    
   return NULL;
}

int main(void)
{
   clock_t initial, end;
   double total_time;
   cout << "number of darts:" << endl;
   cin >> darts_scope ;
   cout << "number of threads:" << endl;
   cin >> threads_number ;

   initial = clock();
   pthread_t tid[threads_number];
   int spilt[threads_number];

   for(int i = 0; i < threads_number; i++)
      spilt[i] = i;
    
   for(int j = 0; j < threads_number; j++)
      pthread_create(&tid[j], NULL, add_darts, &spilt[j]);
    
   for(int k = 0; k < threads_number; k++)
      pthread_join(tid[k], NULL);
    
   final_pi = 4 * final_pi / darts_scope;
   end = clock();
   printf("%.20lf\n", final_pi);
   total_time = (double)(end - initial) / CLOCKS_PER_SEC;
   printf("total time is: %fs\n", total_time);
    
   return 0;
}
