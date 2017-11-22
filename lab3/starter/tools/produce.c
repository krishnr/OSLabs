/**
 * @file:   producer.c
 * @brief: 
 * @date:   2017/11/22
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./sender.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 * Check /dev/mqueue to clean up the queue if sender fails to do so.
 */

// compile with gcc produce.c -lrt -lm -o produce

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

void producer(int id) {
    int i;
    int j;
    int k = -1;
    mqd_t q = mq_open(Q_NAME, O_WRONLY);
    if (q == (mqd_t) -1) {
        perror("Producer - mq_open");
        exit(1);
    }

    for (i = 0; i < N; i++) {
        if (i%P == id) {
            if (mq_send(q,(char *) &i, sizeof(int), 0) == -1) {
                perror("mq_send() failed");
                exit(1);
            }
        }
    }

    if (C > P) {
        for (j = 0; j < C; j++) {
            // Terminate consumers
            if (mq_send(q,(char *) &k, sizeof(int), 0) == -1) {
                perror("mq_send() failed");
                exit(1);
            }
        }
    }
    else {
        // Terminate single consumer
        if (mq_send(q,(char *) &k, sizeof(int), 0) == -1) {
            perror("mq_send() failed");
            exit(1);
        }
    }

    exit(0);
}


void consumer(int cid) {
    mqd_t q = mq_open(Q_NAME, O_RDWR);
    if (q == (mqd_t) -1) {
        perror("Producer - mq_open");
        exit(1);
    }

    while (true){
        int pt;
        int root;

        struct timespec ts = {time(0) + 5, 0};

        /* only block for a limited time if the queue is empty */
        if (mq_timedreceive(q, (char *) &pt, sizeof(int), 0, &ts) == -1) {
            perror("mq_timedreceive() failed");
            printf("Consumer: %d failed.\n", cid);
            exit(1);
        } else {
            if (pt < 0) {
                break;
            }
            root = sqrt(pt);
            if (root*root == pt) {
                printf("%d %d %d\n", cid, pt, root);
            }
        }
    }
    exit(0);
}


int main(int argc, char *argv[])
{
    struct timeval tv;
    double t1;
    double t2;
    gettimeofday(&tv, NULL);
    t1 = tv.tv_sec + tv.tv_usec/1000000.0;


    mqd_t qdes;

    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;

    int i;

    if ( argc !=5 ) {
        printf("Usage: %s <N> <B> <P> <C> \n", argv[0]);
        exit(1);
    }

    N = atoi(argv[1]);
    B = atoi(argv[2]);
    P = atoi(argv[3]);
    C = atoi(argv[4]);

    attr.mq_maxmsg = B;
    attr.mq_msgsize = sizeof(int);
    attr.mq_flags = 0;

    qdes = mq_open(Q_NAME, O_RDWR | O_CREAT, mode, &attr);

    if (qdes == (mqd_t) -1) {
        perror("mq_open() failed");
        return 1;
    }

    // Create producers and consumers
    pid_t* prod_pids = malloc(P * sizeof(pid_t));
    pid_t* con_pids = malloc(C * sizeof(pid_t));


    // produce P producers (fork) that each send i%P == id to the queue
    for (i = 0; i < P; i++) {
        prod_pids[i] = fork();
        if (prod_pids[i] < 0) {
            printf("Producer %d fork failed\n", i);
        } else if (prod_pids[i] == 0) {
            producer(i);
            break;
        }
    }

    // C consumers
    for (i = 0; i < C; i++) {
        con_pids[i] = fork();
        if (con_pids[i] < 0) {
            printf("Consumer %d fork failed\n", i);
        } else if (con_pids[i] == 0) {
            consumer(i);
            break;
        }
    }

    int* ret;
    for(i = 0; i < P; i++) {
        waitpid(prod_pids[i], ret, 0);
    }

    for(i = 0; i < P; i++) {
        waitpid(con_pids[i], ret, 0);
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
