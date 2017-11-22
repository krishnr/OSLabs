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


void producer(int id, int N, int P, int C, mqd_t qdes) {
    int i;
    int j;
    int k = -1;

    for (i = 0; i < N; i++) {
        if (i%P == id) {
            if (mq_send(qdes,(char *) &i, sizeof(int), 0) == -1) {
                perror("mq_send() failed");
            }
        }
    }

    if (C > P) {
        for (j = 0; j < C; j++) {
            // Terminate consumers
            if (mq_send(qdes,(char *) &k, sizeof(int), 0) == -1) {
                perror("mq_send() failed");
            }
        }
    }
    else {
        // Terminate single consumer
        if (mq_send(qdes,(char *) &k, sizeof(int), 0) == -1) {
            perror("mq_send() failed");
        }
    }

    return;
}


void consumer(int cid, mqd_t qdes) {
    bool c_continue = true;
    while (c_continue){
        int pt;
        float root;
        int newRoot;

        struct timespec ts = {time(0) + 5, 0};

        /* only block for a limited time if the queue is empty */
        if (mq_timedreceive(qdes, (char *) &pt, \
		    sizeof(int), 0, &ts) == -1) {
            perror("mq_timedreceive() failed");
            printf("Type Ctrl-C and wait for 5 seconds to terminate.\n");
        } else {
            root = sqrt(pt);
            newRoot = root;
            if (newRoot == pt) {
                printf("%d %d %d\n", cid, pt, root);
            }
        }
    }
}


int main(int argc, char *argv[])
{
    mqd_t qdes;
    char qname[]="/mailbox_kramesh_producer";

    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;

    int N;
    int B;
    int P;
    int C;
    int i;

    pid_t pid;
    pid_t cid;

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

    qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);

    if (qdes == -1) {
        perror("mq_open() failed");
        exit(1);
    }


    // produce P producers (fork) that each send i%P == id to the queue
    for (i = 0; i < P; i++) {
        pid = fork();
        if (pid < 0) {
            printf("Fork failed");
        } else if (pid == 0) {
            producer(i, N, P, C, qdes);
            break;
        }
    }

    // C consumers
    for (i = 0; i < C; i++) {
        cid = fork();
        if (cid < 0) {
            printf("Fork failed");
        } else if (cid == 0) {
            consumer(i, qdes);
            break;
        }
    }

    /*
    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    if (mq_unlink(qname) != 0) {
        perror("mq_unlink() failed");
        exit(3);
    }
    */

    return 0;
}
