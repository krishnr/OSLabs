/**
 * @file: produce.c
 * @brief:  send random points to a POSIX queue. Type 'q' to terminate. 
 * @date:   2017/11/01
 * NOTES:
 * To execute: <executable> <arg1>, where arg1 is a posix queue name.
 * EXAMPLE: ./sender.out /mailbox1_userid 
 * A valid posix queue name must start with a "/". 
 * Execute command "man mq_overview" for details.
 * Check /dev/mqueue to clean up the queue if sender fails to do so.
 */
