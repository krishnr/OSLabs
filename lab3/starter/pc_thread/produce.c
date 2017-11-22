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

int N; // Amount of numbers
int B; // Buffer size
int P; // Number of producers
int C; // Number of consumers

void *producer(void * arg);
void *consumer(void * arg);

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

    int shared_buffer[B];

    // Create producers and consumers
    pthread_t producers[P];
    pthread_t consumers[C];

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

    void* ret_val;
    for(i = 0; i < P; i++) {
        pthread_join(producers[i], &ret_val);
        free(ret_val);
    }

    // Terminate all consumers
    // int kill = -1;
    // for (i = 0; i < C; i++) {
    //     if (mq_send(qdes,(char *) &kill, sizeof(int), 0) == -1) {
    //         perror("Main - mq_send() failed");
    //         exit(1);
    //     }
    // }

    for(i = 0; i < C; i++) {
        pthread_join(consumers[i], &ret_val);
        free(ret_val);
    }

    return 0;
}

void *producer(void* arg) {
    int* pid = (int *) arg;
    printf("Producer %d says hello!\n", *pid);
    
    free(arg);
    pthread_exit(0);
}


void *consumer(void* arg) {
    int* cid = (int *) arg;
    printf("Consumer %d says hello!\n", *cid);
    
    free(arg);
    pthread_exit(0);
}
