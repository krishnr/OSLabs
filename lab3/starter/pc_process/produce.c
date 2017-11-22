/**
 * @file:   produce.c
 * @brief: 
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

    mqd_t qdes;
    struct mq_attr attr;
    attr.mq_maxmsg = B;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;
    attr.mq_curmsgs = 0;

    qdes = mq_open(Q_NAME, (O_CREAT | O_RDWR), (S_IRWXU | S_IRWXG | S_IRWXO), &attr);

    if (qdes == -1) {
        perror("Main - mq_open() failed");
        return 1;
    }

    // Create producers and consumers
    pid_t* prod_pids = malloc(P * sizeof(pid_t));
    pid_t* con_pids = malloc(C * sizeof(pid_t));

    int i;
    // create P producers
    for (i = 0; i < P; i++) {
        prod_pids[i] = fork();
        if (prod_pids[i] < 0) {
            printf("Producer %d fork failed\n", i);
        } else if (prod_pids[i] == 0) {
            producer(i);
            break;
        }
    }

    // create C consumers
    for (i = 0; i < C; i++) {
        con_pids[i] = fork();
        if (con_pids[i] < 0) {
            printf("Consumer %d fork failed\n", i);
        } else if (con_pids[i] == 0) {
            consumer(i);
            break;
        }
    }

    // wait for all producer and consumer threads to exit
    // before closing the mq
    int* ret_val;
    for(i = 0; i < P; i++) {
        waitpid(prod_pids[i], ret_val, 0);
    }

    // Terminate all consumers
    int kill = -1;
    for (i = 0; i < C; i++) {
        if (mq_send(qdes,(char *) &kill, sizeof(int), 0) == -1) {
            perror("Main - mq_send() failed");
            exit(1);
        }
    }

    for(i = 0; i < C; i++) {
        waitpid(con_pids[i], ret_val, 0);
    }


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

    gettimeofday(&tv, NULL);

    t2 = tv.tv_sec + tv.tv_usec/1000000.0;
    printf("System execution time: %.6lf seconds\n", t2-t1);

    return 0;
}

void producer(int id) {
    int i;
    mqd_t q = mq_open(Q_NAME, O_WRONLY);
    if (q == -1) {
        perror("Producer - mq_open() failed");
        exit(1);
    }

    for (i = 0; i < N; i++) {
        if (i%P == id) {
            if (mq_send(q,(char *) &i, sizeof(int), 0) == -1) {
                perror("Producer - mq_send() failed");
                exit(1);
            }
        }
    }

    mq_close(q);
    exit(0);
}


void consumer(int cid) {
    mqd_t q = mq_open(Q_NAME, O_RDWR);
    if (q == -1) {
        perror("Consumer - mq_open() failed");
        exit(1);
    }

    int pt;
    int root;
    while (true){
        if (mq_receive(q, (char *) &pt, sizeof(int), NULL) == -1) {
            perror("mq_receive() failed");
            printf("Consumer: %d failed.\n", cid);
            exit(1);
        } else {
            
            // Encounters kill signal
            if (pt < 0) {
                break;
            }

            root = sqrt(pt);
            if (root*root == pt) {
                printf("%d %d %d\n", cid, pt, root);
            }
        }
    }

    mq_close(q);
    exit(0);
}

