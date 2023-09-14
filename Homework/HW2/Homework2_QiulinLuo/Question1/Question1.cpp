#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>

#define P_NUMBER 5

void thinking(int id);
void eating(int id);
void forksUP(int id);
void forksDOWN(int id);
void* ThreadsDO(void *arg);

sem_t forks[P_NUMBER];
sem_t deadlock_judgement;
pthread_t tid[P_NUMBER];

int main(){
    int id[P_NUMBER];
    
    sem_init(&deadlock_judgement, 0, P_NUMBER - 1);

    for (int i = 0; i < P_NUMBER; i++)
        sem_init(&forks[i], 0, 1);
    
    for(int j = 0; j < P_NUMBER; j++){
        id[j] = j;
        pthread_create(&tid[j], NULL, ThreadsDO, (void*)(&id[j]));
    }
    while(1)
    for (int k = 0; k < P_NUMBER; k++)
        sem_destroy(&forks[k]);
    
    exit(0);
    return 0;
}

void thinking(int id){
    sleep(10);
    printf("philosopher:%d is thinking\n", id);
}

void eating(int id){
    sleep(10);
    printf("philosopher:%d is eating\n", id);
}

void forksUP(int id){
    int left = (id + P_NUMBER - 1) % P_NUMBER;
    int right = (id + 1) % P_NUMBER;
    printf("philosopher:%d uses left fork:%d and right fork:%d\n", id, left, right);
    sem_wait(&forks[left]);
    sem_wait(&forks[right]);
    printf("philosopher:%d takes forks\n", id);
}

void forksDOWN(int id){
    int left = (id + P_NUMBER - 1) % P_NUMBER;
    int right = (id + 1) % P_NUMBER;
    printf("philosopher:%d puts down forks\n", id);
    sem_post(&forks[left]);
    sem_post(&forks[right]);
}

void* ThreadsDO(void *arg){
    int id = *(int*)arg;
    while(1){
        thinking(id);
        sem_wait(&deadlock_judgement);
        forksUP(id);
        sem_post(&deadlock_judgement);
        eating(id);
        forksDOWN(id);
    }
}