#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>

#define N 10
sem_t semA, semB, semC;

void *thread_A(void *arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semA);
        printf("A %d\n", i);
        sem_post(&semB);
    }
    return NULL;
}

void *thread_B(void *arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semB);
        printf("B %d\n", i);
        sem_post(&semC);
    }
    return NULL;
}

void *thread_C(void *arg) {
    for (int i = 0; i < N; i++) {
        sem_wait(&semC);
        printf("C %d\n", i);
        sem_post(&semA);
    }
    return NULL;
}

int main() {
    pthread_t tidA, tidB, tidC;
    
    if (sem_init(&semA, 0, 1) != 0) {
        perror("sem_init semA failed");
        exit(1);
    }
    if (sem_init(&semB, 0, 0) != 0) {
        perror("sem_init semB failed");
        exit(1);
    }
    if (sem_init(&semC, 0, 0) != 0) {
        perror("sem_init semC failed");
        exit(1);
    }
    
    if (pthread_create(&tidA, NULL, thread_A, NULL) != 0) {
        perror("pthread_create A failed");
        exit(1);
    }
    if (pthread_create(&tidB, NULL, thread_B, NULL) != 0) {
        perror("pthread_create B failed");
        exit(1);
    }
    if (pthread_create(&tidC, NULL, thread_C, NULL) != 0) {
        perror("pthread_create C failed");
        exit(1);
    }
    
    pthread_join(tidA, NULL);
    pthread_join(tidB, NULL);
    pthread_join(tidC, NULL);
    
    sem_destroy(&semA);
    sem_destroy(&semB);
    sem_destroy(&semC);
    return 0;
}
