#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#define DEPOSITS 4
#define WITHDRAWS 4
#define N 10

long long balance = 0;
pthread_mutex_t mutex;
pthread_spinlock_t spinlock;

typedef enum { MUTEX, SPINLOCK } Type;
Type type;
typedef enum { SHORT, LONG } Length;
Length len;

void* deposit(void* arg){
        for(int i = 0; i<N; i++){
                if(type == MUTEX){
			pthread_mutex_lock(&mutex);
                        balance++;
			if(len == LONG){
				usleep(100);
			}
			pthread_mutex_unlock(&mutex);
                } else if(type == SPINLOCK){
                        pthread_spin_lock(&spinlock);
                        balance++;
                        if(len == LONG){
                                usleep(100);
                        }
                        pthread_spin_unlock(&spinlock);
                } else {
			printf("Error of type");
		}

        }
        return NULL;
}


void* withdraw(void* arg){
        for(int i = 0; i<N; i++){
                if(type == MUTEX){
                        pthread_mutex_lock(&mutex);
                        balance--;
                        if(len == LONG){
                                usleep(100);
                        }
                        pthread_mutex_unlock(&mutex);
                } else if(type == SPINLOCK){
                        pthread_spin_lock(&spinlock);
                        balance--;
                        if(len == LONG){
                                usleep(100);
                        }
                        pthread_spin_unlock(&spinlock);
                } else {
                        printf("Error of type");
                }

        }
        return NULL;
}

int main(int argc, char* argv[]) {
	if (strcmp(argv[1], "mutex") == 0) {
                type = MUTEX;
                pthread_mutex_init(&mutex, NULL);
                printf("Inside mutex\n");
        }
        else if (strcmp(argv[1], "spinlock") == 0) {
                type = SPINLOCK;
                pthread_spin_init(&spinlock, PTHREAD_PROCESS_PRIVATE);
                printf("Inside spinlock\n");
        } else {
                printf("Error in type");
                return 1;
        }
        if (strcmp(argv[2], "short") == 0) {
                len = SHORT;
                printf("Inside short section\n");
        }
        else if (strcmp(argv[2], "long") == 0) {
                len = LONG;
                printf("Inside long section\n");
        } else {
                printf("Error in length");
                return 1;
        }

        pthread_t deposits[DEPOSITS];
	pthread_t withdraws[WITHDRAWS];
        balance = 0;
	struct timespec start, end;
	clock_gettime(CLOCK_MONOTONIC, &start);

        for (int i = 0; i < DEPOSITS; i++) {
                pthread_create(&deposits[i], NULL, deposit, NULL);
        }
        for (int i = 0; i < DEPOSITS; i++) {
                pthread_join(deposits[i], NULL);
        }
        for (int i = 0; i < WITHDRAWS; i++) {
                pthread_create(&withdraws[i], NULL, withdraw, NULL);
        }
        for (int i = 0; i < WITHDRAWS; i++) {
                pthread_join(withdraws[i], NULL);
        }
	clock_gettime(CLOCK_MONOTONIC, &end);
    	double time = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        printf("Balance %lld\n", balance);
        printf("Time %.3f\n", time);

        if (type == MUTEX) {
                pthread_mutex_destroy(&mutex);
        } else if (type == SPINLOCK) {
                pthread_spin_destroy(&spinlock);
        }

        return 0;
}
