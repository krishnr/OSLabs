/**
 * @file:   produce.c
 * @brief: 
 * @date:   2017/11/22
 * NOTES:
    Compile with: gcc produce.c -pthread -lm -o produce
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

int N; // Amount of numbers
int B; // Buffer size
int P; // Number of producers
int C; // Number of consumers

void *producer(void * arg);
void *consumer(void * arg);

int* shared_buffer;

sem_t items;
sem_t spaces;
pthread_mutex_t mutex;

int p_index = 0;
int c_index = 0;

int main(int argc, char *argv[])
{
    struct timeval tv;
    double t1;
    double t2;
    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec + tv.tv_usec/1000000.0;

    if ( argc !=5 ) {
        printf("Usage: %s <N> <B> <P> <C> \n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    B = atoi(argv[2]);
    P = atoi(argv[3]);
    C = atoi(argv[4]);

    shared_buffer = malloc(B * sizeof(int));

    // Create producers and consumers
    pthread_t producers[P];
    pthread_t consumers[C];

    sem_init(&items, 0, 0);
    sem_init(&spaces, 0, B);
    pthread_mutex_init(&mutex, NULL);

    int i;
    // create P producers
    for (i = 0; i < P; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        int status = pthread_create(&producers[i], NULL, producer, id);
        if (status != 0) {
            printf("Producer %d thread create failed\n", i);
        }
    }

    // create C consumers
    for (i = 0; i < C; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        int status = pthread_create(&consumers[i], NULL, consumer, id);
        if (status != 0) {
            printf("Consumer %d thread create failed\n", i);
        }
    }

    // join all producers and consumers
    void* ret_val;
    for(i = 0; i < P; i++) {
        pthread_join(producers[i], &ret_val);
        free(ret_val);
    }
    for(i = 0; i < C; i++) {
        pthread_join(consumers[i], &ret_val);
        free(ret_val);
    }

    pthread_mutex_destroy(&mutex);
    sem_destroy(&items);
    sem_destroy(&spaces);
    
    free(shared_buffer);

    gettimeofday(&tv, NULL);

    t2 = tv.tv_sec + tv.tv_usec/1000000.0;
    printf("System execution time: %.6lf seconds\n", t2-t1);

    return 0;
}

void *producer(void* arg) {
    int* pid = (int *) arg;

    int i;
    for (i = 0; i < N; i++) {
        if (i%P == *pid) {
            sem_wait(&spaces);
            pthread_mutex_lock(&mutex);
            shared_buffer[p_index % B] = i;
            p_index++;
            pthread_mutex_unlock(&mutex);
            sem_post(&items);
        }
    }
    
    free(arg);
    pthread_exit(0);
}


void *consumer(void* arg) {
    int* cid = (int *) arg;

    int num;
    int root;
    while(true) {
        sem_wait(&items);
        pthread_mutex_lock(&mutex);
        
        num = shared_buffer[c_index % B];
        c_index++;

        // Read N items so break
        if (c_index >= N-1) {
            break;
        }
        
        pthread_mutex_unlock(&mutex);
        sem_post(&spaces);

        root = sqrt(num);
        if (root*root == num) {
            printf("%d %d %d\n", *cid, num, root);
        }
    }
    sem_post(&spaces);
    sem_post(&items);
    pthread_mutex_unlock(&mutex);

    free(arg);
    pthread_exit(0);
}
