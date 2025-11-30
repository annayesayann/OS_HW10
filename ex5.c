#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define K 3
#define N 10

sem_t printers;
pthread_mutex_t counter_mutex;

int active_printers = 0;

void* print_job(void* arg) {
    long thread_id = (long)arg;

    if (sem_wait(&printers) != 0) {
        perror("sem_wait failed");
        return NULL;
    }

    pthread_mutex_lock(&counter_mutex);
    active_printers++;
    printf("Printing, Active: %d\n", active_printers);
    pthread_mutex_unlock(&counter_mutex);
    
    usleep(1000);
    
    pthread_mutex_lock(&counter_mutex);
    active_printers--;
    pthread_mutex_unlock(&counter_mutex);

    if (sem_post(&printers) != 0) {
        perror("sem_post failed");
        return NULL;
    }

    return NULL;
}

int main() {
    pthread_t jobs[N];
    if (sem_init(&printers, 0, K) != 0) {
        perror("sem_init failed");
        return 1;
    }
    if (pthread_mutex_init(&counter_mutex, NULL) != 0) {
        perror("mutex_init failed");
        return 1;
    }

    for (long i = 0; i < N; i++) {
        if (pthread_create(&jobs[i], NULL, print_job, (void*)(i + 1)) != 0) { 
            perror("Thread creation failed");
            return 1;
        }
    }

    for (int i = 0; i < N; i++) {
        pthread_join(jobs[i], NULL);
    }
    
    sem_destroy(&printers);
    pthread_mutex_destroy(&counter_mutex);
    
    return 0;
}
