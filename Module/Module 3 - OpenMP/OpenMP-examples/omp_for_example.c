#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <iomanip>
#include <time.h>

using namespace std;

int darts_scope;
int threads_number;
double final_pi = 0.0;
pthread_mutex_t mutex;

void *add_darts(void *arg){
    unsigned int seed = 123;
    int spilt_darts = darts_scope / threads_number;
    double this_thread_count = 0.0;
    for(int i = 0; i < spilt_darts; i++){
        double edge_1 = (double)rand_r(&seed) / RAND_MAX;
        double edge_2 = (double)rand_r(&seed) / RAND_MAX;
        if(edge_1 * edge_1 + edge_2 * edge_2 <= 1.0){
            this_thread_count++;
        }
    }
    pthread_mutex_lock(&mutex);
    final_pi += this_thread_count;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(){
  clock_t initial, end;
  double total_time;
  cout << "number of darts:" << endl;
  cin >> darts_scope ;
  cout << "number of threads:" << endl;
  cin >> threads_number ;

  pthread_t tid[threads_number];
  int spilt[threads_number];

  initial = clock();

  for(int i = 0; i < threads_number; i++){
      spilt[i] = i;
      pthread_create(&tid[i], NULL, add_darts, &spilt[i]);
  }
  for(int j = 0; j < threads_number; j++){
      pthread_join(tid[j], NULL);
  }
  final_pi = 4*final_pi/darts_scope;
  
  end = clock();
  total_time = (double)(end - initial) / CLOCKS_PER_SEC;
  printf("total time is: %fs\n", total_time);
  printf("%.20lf\n", final_pi);
  return 0;
}

