/**
 * @file:   produce.c
 * @brief:  Implementing the producer-consumer paradigm with multiple processes and message queues.
 * @date:   2017/11/22
 * NOTES:
    Compile with: gcc produce.c -lrt -lm -o produce
 */


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

const char* Q_NAME = "/sqrt_q";

int N; // Amount of numbers
int B; // Buffer size
int P; // Number of producers
int C; // Number of consumers

void producer(int id);
void consumer(int id);

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

    // Initializing the message queue attributes
    mqd_t qdes;
    struct mq_attr attr;
    attr.mq_maxmsg = B;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;

    // Creating the queue and setting the mode permissions to all users
    qdes = mq_open(Q_NAME, (O_CREAT | O_RDWR), (S_IRWXU | S_IRWXG | S_IRWXO), &attr);

    // If queue creation failed
    if (qdes == -1) {
        perror("Main - mq_open() failed");
        return 1;
    }

    // Store producer and consumer pids
    pid_t* prod_pids = malloc(P * sizeof(pid_t));
    pid_t* con_pids = malloc(C * sizeof(pid_t));

    int i;
    // Create P producer processes
    for (i = 0; i < P; i++) {
        prod_pids[i] = fork();
        if (prod_pids[i] < 0) {
            printf("Producer %d fork failed\n", i);
        } else if (prod_pids[i] == 0) {
            producer(i);
            break;
        }
    }

    // Create C consumer processes
    for (i = 0; i < C; i++) {
        con_pids[i] = fork();
        if (con_pids[i] < 0) {
            printf("Consumer %d fork failed\n", i);
        } else if (con_pids[i] == 0) {
            consumer(i);
            break;
        }
    }

    // Wait for all producers to finish
    int* ret_val;
    for(i = 0; i < P; i++) {
        waitpid(prod_pids[i], ret_val, 0);
    }

    // Send -1 kill signals to the queue to terminate all consumers
    int kill = -1;
    for (i = 0; i < C; i++) {
        if (mq_send(qdes,(char *) &kill, sizeof(int), 0) == -1) {
            perror("Main - mq_send() failed");
            exit(1);
        }
    }

    // Wait for all consumers to finish
    for(i = 0; i < C; i++) {
        waitpid(con_pids[i], ret_val, 0);
    }

    // Close the queue and do cleanup
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    if (mq_unlink(Q_NAME) != 0) {
        perror("mq_unlink() failed");
        exit(3);
    }

    free(prod_pids);
    free(con_pids);

    // Store the finish time and print the execution time
    gettimeofday(&tv, NULL);

    t2 = tv.tv_sec + tv.tv_usec/1000000.0;
    printf("System execution time: %.6lf seconds\n", t2-t1);

    return 0;
}

void producer(int id) {
    int i;
    
    // Open queue for writing 
    mqd_t q = mq_open(Q_NAME, O_WRONLY);
    if (q == -1) {
        perror("Producer - mq_open() failed");
        exit(1);
    }

    // Send ints to the queue according to i%P == id
    for (i = 0; i < N; i++) {
        if (i%P == id) {
            if (mq_send(q,(char *) &i, sizeof(int), 0) == -1) {
                perror("Producer - mq_send() failed");
                exit(1);
            }
        }
    }

    // Close the queue and exit the process
    mq_close(q);
    exit(0);
}


void consumer(int cid) {
    // Open the queue for reading
    mqd_t q = mq_open(Q_NAME, O_RDWR);
    if (q == -1) {
        perror("Consumer - mq_open() failed");
        exit(1);
    }

    int pt;
    int root;
    while (true){
        // If there's an error receiving from queue
        if (mq_receive(q, (char *) &pt, sizeof(int), NULL) == -1) {
            perror("mq_receive() failed");
            printf("Consumer: %d failed.\n", cid);
            exit(1);
        } else {
            // Else the consumer read the point correctly
            
            // Encounters kill signal so break out of loop
            if (pt < 0) {
                break;
            }

            // Calculate and print sqroot
            root = sqrt(pt);
            if (root*root == pt) {
                printf("%d %d %d\n", cid, pt, root);
            }
        }
    }

    // Close the queue and exit the process
    mq_close(q);
    exit(0);
}

