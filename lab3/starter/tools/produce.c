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


void producer(int id, int N, int P, mqd_t qdes) {
    int i;

    for (i = 0; i < N; i++) {
        if (i%P == id) {
            if (mq_send(qdes,(char *) &i, sizeof(int), 0) == -1) {
                perror("mq_send() failed");
            }
        }
    }

    return;
}


void consumer(int cid, int N, mqd_t qdes, mqd_t cqdes) {
    struct mq_attr pq_attr;
    mq_getattr(qdes, &pq_attr);

    printf("%d \n", pq_attr.mq_curmsgs);
    /* while(
    int pt;
    struct timespec ts = {time(0) + 5, 0};

    // only block for a limited time if the queue is empty
    if (mq_timedreceive(qdes, (char *) &pt, \
        sizeof(int), 0, &ts) == -1) {
        perror("mq_timedreceive() failed");
    } */
}


int main(int argc, char *argv[])
{
    mqd_t qdes;
    mqd_t cqdes;
    char qname[]="/mailbox_kramesh_producer";
    char cqname[]="/mailbox_kramesh_consumer";
    
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

    cqdes = mq_open(cqname, O_RDWR | O_CREAT, mode, &attr);

    if (cqdes == -1) {
        perror("cq: mq_open() failed");
        exit(1);
    }

    // produce P producers (fork) that each send i%P == id to the queue
    for (i = 0; i < P; i++) {
        pid = fork();
        if (pid < 0) {
            printf("Fork failed");
        } else if (pid == 0) {
            producer(i, N, P, qdes);
            break;
        }
    }
    
    // C consumers
    for (i = 0; i < C; i++) {
        cid = fork();
        if (cid < 0) {
            printf("Fork failed");
        } else if (cid == 0) {
            consumer(i, N, qdes, cqdes);
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
