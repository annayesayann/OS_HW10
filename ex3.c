#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define BUFFER_SIZE 8
#define P 3
#define C 2
#define K 5
#define TOTAL_ITEMS (P * K)
#define POISON_PILL -1

int buffer[BUFFER_SIZE];
int in_pos = 0;
int out_pos = 0;

sem_t empty_slots;
sem_t full_slots;
pthread_mutex_t buffer_mutex;

int current_count = 0;

void* producer(void* arg) {
    long id = (long)arg;
    for (int i = 0; i < K; i++) {
        sem_wait(&empty_slots);
        pthread_mutex_lock(&buffer_mutex);
        
        buffer[in_pos] = (int)id + 1;
        in_pos = (in_pos + 1) % BUFFER_SIZE;
	current_count++;
	printf("After producing: %d items in the buffer\n", current_count);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&full_slots);
    }
    return NULL;
}

void* consumer(void* arg) {
    long id = (long)arg;
    while (1) {
        sem_wait(&full_slots);
        pthread_mutex_lock(&buffer_mutex);
        
        int item_value = buffer[out_pos];
        // Starts putting end signals to claim consumers waiting inside semaphore 
        if (item_value == POISON_PILL) {
            pthread_mutex_unlock(&buffer_mutex);
            sem_post(&full_slots);
            break;
        }
	buffer[out_pos] = 0;
        out_pos = (out_pos + 1) % BUFFER_SIZE;
	current_count--;
	printf("After consuming: %d items in the buffer\n", current_count);
        pthread_mutex_unlock(&buffer_mutex);
        sem_post(&empty_slots);
    }
    return NULL;
}


int main() {
    pthread_t producers[P];
    pthread_t consumers[C];
    
    sem_init(&empty_slots, 0, BUFFER_SIZE);
    sem_init(&full_slots, 0, 0);
    pthread_mutex_init(&buffer_mutex, NULL);
    
    for (long i = 0; i < P; i++) {
        pthread_create(&producers[i], NULL, producer, (void*)(i + 1));
    }
    
    for (long i = 0; i < C; i++) {
        pthread_create(&consumers[i], NULL, consumer, (void*)(i + 1));
    }
    
    for (int i = 0; i < P; i++) {
        pthread_join(producers[i], NULL);
    }
    
    for (int i = 0; i < C; i++) {
        sem_wait(&empty_slots);
        
        pthread_mutex_lock(&buffer_mutex);
        buffer[in_pos] = POISON_PILL;
        in_pos = (in_pos + 1) % BUFFER_SIZE;
        pthread_mutex_unlock(&buffer_mutex);
        
        sem_post(&full_slots);
    }

    for (int i = 0; i < C; i++) {
        pthread_join(consumers[i], NULL);
    }
    
    
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    pthread_mutex_destroy(&buffer_mutex);
    
    return 0;
}
