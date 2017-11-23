/**
 * @file:   produce.c
 * @brief:  Implementing the producer-consumer paradigm with multiple threads and shared memory.
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

// Shared circular buffer that holds the messages
int* shared_buffer;

// 2 semaphores to coordinate consumers and producers
sem_t items;
sem_t spaces;
// 1 mutex to protect access to shared memory
pthread_mutex_t mutex;

// Shared indexes that keep track of where the all the producers/consumers are writing/reading
int p_index = 0;
int c_index = 0;

int main(int argc, char *argv[])
{
    // Storing start time as t1
    struct timeval tv;
    double t1;
    double t2;
    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec + tv.tv_usec/1000000.0;

    // Checking that all the args are passed in
    if ( argc !=5 ) {
        printf("Usage: %s <N> <B> <P> <C> \n", argv[0]);
        exit(1);
    }

    // Converting the args to ints
    N = atoi(argv[1]);
    B = atoi(argv[2]);
    P = atoi(argv[3]);
    C = atoi(argv[4]);

    // Setting the shared buffer to hold B integers
    shared_buffer = malloc(B * sizeof(int));

    // Storing thread ids of producers and consumers
    pthread_t producers[P];
    pthread_t consumers[C];

    // Initializing items to 0 as no items produced to start off
    sem_init(&items, 0, 0);
    // Initializing spaces to B as all the buffer spaces empty to start off
    sem_init(&spaces, 0, B);
    // Initializing mutex
    pthread_mutex_init(&mutex, NULL);

    int i;
    // create P producer threads
    for (i = 0; i < P; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        int status = pthread_create(&producers[i], NULL, producer, id);
        if (status != 0) {
            printf("Producer %d thread create failed\n", i);
        }
    }

    // create C consumer threads
    for (i = 0; i < C; i++) {
        int* id = malloc(sizeof(int));
        *id = i;
        int status = pthread_create(&consumers[i], NULL, consumer, id);
        if (status != 0) {
            printf("Consumer %d thread create failed\n", i);
        }
    }

    // Join all producers and consumers
    void* ret_val;
    for(i = 0; i < P; i++) {
        pthread_join(producers[i], &ret_val);
        free(ret_val);
    }
    for(i = 0; i < C; i++) {
        pthread_join(consumers[i], &ret_val);
        free(ret_val);
    }

    // Cleanup
    pthread_mutex_destroy(&mutex);
    sem_destroy(&items);
    sem_destroy(&spaces);
    
    free(shared_buffer);

    // Store the finish time and print the execution time
    gettimeofday(&tv, NULL);

    t2 = tv.tv_sec + tv.tv_usec/1000000.0;
    printf("System execution time: %.6lf seconds\n", t2-t1);

    return 0;
}

void *producer(void* arg) {
    int* pid = (int *) arg;

    int i;
    // Add ints to the buffer according to i%P == pid
    for (i = 0; i < N; i++) {
        if (i%P == *pid) {
            sem_wait(&spaces);
            pthread_mutex_lock(&mutex);

            // Write to p_index % B (circular buffer)
            shared_buffer[p_index % B] = i;
            p_index++;
            
            pthread_mutex_unlock(&mutex);
            sem_post(&items);
        }
    }
    
    // Cleanup and exit thread
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
        
        // Read from c_index % B (circular buffer)
        num = shared_buffer[c_index % B];
        c_index++;

        // Already consumed N items so break
        if (c_index >= N-1) {
            break;
        }
        
        pthread_mutex_unlock(&mutex);
        sem_post(&spaces);

        // Calculate and print sqrt
        root = sqrt(num);
        if (root*root == num) {
            printf("%d %d %d\n", *cid, num, root);
        }
    }
    // Thread broke out of loop because N items have been read
    // Post semaphores and unlock mutex to let any stuck threads through
    sem_post(&spaces);
    sem_post(&items);
    pthread_mutex_unlock(&mutex);

    // Cleanup and exit thread
    free(arg);
    pthread_exit(0);
}
