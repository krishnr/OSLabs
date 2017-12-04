/**
  * ECE254 Linux Dynamic Memory Management Lab
  * @file: main_test.c
  * @brief: The main file to write tests of memory allocation algorithms
  */ 

/* includes */
/* system provided header files. You may add more */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* non-system provided header files. 
   Do not include more user-defined header files here
 */
#include "mem.h"

/*int main(int argc, char *argv[]) {
//    best_fit_memory_init(1024);
//    int* p = best_fit_alloc(8);
//    int num;
//    if (p != NULL) {
//        printf("Size: %d\n", *(p-1));
//        num = best_fit_count_extfrag(8);
//	    printf("num = %d\n", num);
//        best_fit_dealloc(p);
//        num = best_fit_count_extfrag(8);
//	    printf("num = %d\n", num);
//    } else {
//        printf("NULL pointer returned\n");
//    }

    worst_fit_memory_init(1024);
    int* p1 = worst_fit_alloc(8);
    int num1;
    if (p1 != NULL) {
        printf("Size: %d\n", *(p1-1));
        num1 = worst_fit_count_extfrag(8);
        printf("num = %d\n", num1);
        worst_fit_dealloc(p1);
        num1 = worst_fit_count_extfrag(8);
        printf("num = %d\n", num1);
    } else {
        printf("NULL pointer returned\n");
    }
    return 0;
}
*/
int best_fit_test();

int main(int argc, char *argv[])
{
	int num = 0;
	int algo = 0; // default algorithm to test is best fit
	void *p, *q;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <0/1>. 0 for best fit and 1 for worst fit \n", argv[0]);
		exit (1);
	} else if (!strcmp(argv[1], "1") || !strcmp(argv[1], "0") || !strcmp(argv[1], "2") || !strcmp(argv[1], "3") || !strcmp(argv[1], "4")) {
		algo = atoi(argv[1]);
	} else {
		fprintf(stderr, "Invalid argument, please specify 0 or 1\n");
		exit(1);
	}

	if ( algo == 0 ) {
		best_fit_memory_init(1024);	// initizae 1KB, best fit

		p = best_fit_alloc(8);		// allocate 8B
		printf("best fit: p=%p\n", p);
		if ( p != NULL ) {
			best_fit_dealloc(p);
		}
		num = best_fit_count_extfrag(4);
	} else if ( algo == 1 ) {

		worst_fit_memory_init(1024);	// initizae 1KB, worst fit

		q = worst_fit_alloc(8);		// allocate 8B
		printf("worst fit: q=%p\n", q);
		if ( q != NULL ) {
			worst_fit_dealloc(q);
		}
		num = worst_fit_count_extfrag(4);
	} else if ( algo == 2 ) { // Run best fit test
        best_fit_test();

    } else if ( algo == 3 ) { // Run worst fit test

    } else if ( algo == 4 ) { // Run comparison test

    }

    else {
		fprintf(stderr, "Should not reach here!\n");
		exit(1);
	}

	printf("num = %d\n", num);

	return 0;
}

int best_fit_test() {
    int i;
    best_fit_memory_init(1024);

    void *pointers[10];
    void *q;
    int num;

    for (i = 0; i < 10; i++) {
        if (i % 2 == 0)
            pointers[i] = best_fit_alloc(32);
        else
            pointers[i] = best_fit_alloc(8);
        printf("best fit: p=%p\n", pointers[i]);
    }

    if (pointers[1] != NULL) // Dealloc 8 bytes
        best_fit_dealloc(pointers[1]);

    if (pointers[2] != NULL) // Dealloc 32 bytes
        best_fit_dealloc(pointers[2]);

    if (pointers[5] != NULL) // Dealloc 8 bytes
        best_fit_dealloc(pointers[5]);

    q = best_fit_alloc(4);

    num = best_fit_count_extfrag(32);
    printf("Ext frag: %d", num);

    return 0;
}
