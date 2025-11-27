#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#define N 4
#define M 50000

long long counter = 0;
pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

const char* type = "";

void* func(void* arg){
	for(int i = 0; i<M; i++){
		if(strcmp(type, "nosync") == 0){
			counter++;
		} else if(strcmp(type, "mutex") == 0){
			pthread_mutex_lock(&mutex);
			counter++;
                        pthread_mutex_unlock(&mutex);
		} else if(strcmp(type, "spinlock") == 0){
                        pthread_spin_lock(&spinlock);
                        counter++;
                        pthread_spin_unlock(&spinlock);
                }
	
	}
	return NULL;
}

int main(int argc, char* argv[]) {
	if (strcmp(argv[1], "nosync") == 0) {
		type = "";
		printf("Inside no sync\n");
	}
        else if (strcmp(argv[1], "mutex") == 0) {
                type = "mutex";
        	pthread_mutex_init(&mutex, NULL);
                printf("Inside mutex\n");
        }
        else if (strcmp(argv[1], "spinlock") == 0) {
                type = "spinlock";
                pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
		printf("Inside spinlock\n");
        } else {
		printf("Error in type");
		return 1;
	}
	pthread_t threads[N];
        counter = 0;
	for (int i = 0; i < N; i++) {
        	pthread_create(&threads[i], NULL, func, NULL);
    	}
	for (int i = 0; i < N; i++) {
     		pthread_join(threads[i], NULL);
    	}

	printf("expected %lld\n", (long long) N*M);
	printf("counter %lld\n", counter);

	if (strcmp(type, "mutex") == 0) {
        	pthread_mutex_destroy(&mutex);
    	} else if (strcmp(type, "spinlock") == 0) {
        	pthread_spin_destroy(&spinlock);
    	}

    	return 0;
}

