/**
 * @file:   sender.c
 * @brief:  send random points to a POSIX queue. Type 'q' to terminate. 
 * @date:   2014/06/25
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./sender.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 * Check /dev/mqueue to clean up the queue if sender fails to do so.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>

int main(int argc, char *argv[])
{
    char  quit = '\0';

    if ( argc !=5 ) {
        printf("Usage: %s <N> <B> <P> <C> \n", argv[0]);
        exit(1);
    }
    
    return 0;
}
