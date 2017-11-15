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

int main(int argc, char *argv[])
{
    mqd_t qdes;
    char qname[]="/mailbox_kramesh";
    
    mode_t mode = S_IRUSR | S_IWUSR;
    struct mq_attr attr;

    int N;
    int B;
    int P;
    int C;
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

    qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);

    if (qdes == -1) {
        perror("mq_open() failed");
        exit(1);
    }

    // produce P producers (fork) that each send i%P == id to the queue
    for (i = 0; i < P; i++) {
        printf(" %d \n", i);
    }

    if (mq_close(qdes) == -1) {
        perror("mq_close() failed");
        exit(2);
    }

    if (mq_unlink(qname) != 0) {
        perror("mq_unlink() failed");
        exit(3);
    } 

    
    return 0;
}
