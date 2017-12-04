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

int best_fit_test();
int worst_fit_test();
int comparison_test();

int main(int argc, char *argv[])
{
	int num = 0;
	int algo = 0; // default algorithm to test is best fit
	void *p, *q;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <0/1/2/3/4/5>\n", argv[0]);
		exit (1);
	} else if (!strcmp(argv[1], "1") || !strcmp(argv[1], "0") || !strcmp(argv[1], "2") || !strcmp(argv[1], "3") || !strcmp(argv[1], "4") || !strcmp(argv[1], "5")) {
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
	    printf("num = %d\n", num);
	} else if ( algo == 1 ) {

		worst_fit_memory_init(1024);	// initizae 1KB, worst fit

		q = worst_fit_alloc(8);		// allocate 8B
		printf("worst fit: q=%p\n", q);
		if ( q != NULL ) {
			worst_fit_dealloc(q);
		}
		num = worst_fit_count_extfrag(4);
	    printf("num = %d\n", num);
	} else if ( algo == 2 ) { // Run best fit test
        best_fit_test();
    } else if ( algo == 3 ) { // Run worst fit test
        worst_fit_test();
    } else if ( algo == 4 ) { // Run comparison test
        comparison_test();
    } else if (algo == 5) { // Testing memory inits
        best_fit_memory_init(250);
        best_fit_memory_init(5);
        best_fit_memory_init(1678);
        best_fit_memory_init(1024);
        best_fit_memory_init(4);
        best_fit_memory_init(13);
        best_fit_memory_init(11);
        best_fit_memory_init(12);

        worst_fit_memory_init(250);
        worst_fit_memory_init(5);
        worst_fit_memory_init(1678);
        worst_fit_memory_init(1024);
        worst_fit_memory_init(4);
        worst_fit_memory_init(13);
        worst_fit_memory_init(11);
        worst_fit_memory_init(12);
    }

    else {
		fprintf(stderr, "Should not reach here!\n");
		exit(1);
	}

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
            pointers[i] = best_fit_alloc(31);
        else
            pointers[i] = best_fit_alloc(6);
        printf("best fit: p=%p\n", pointers[i]);
    }

    if (pointers[1] != NULL) // Dealloc 8 bytes
        best_fit_dealloc(pointers[1]);

    if (pointers[2] != NULL) // Dealloc 32 bytes
        best_fit_dealloc(pointers[2]);

    if (pointers[5] != NULL) // Dealloc 8 bytes
        best_fit_dealloc(pointers[5]);

    q = best_fit_alloc(4);

    num = best_fit_count_extfrag(69);
    printf("Ext frag: %d\n", num);

    return 0;
}

int worst_fit_test() {
    int i;
    worst_fit_memory_init(1024);

    void *pointers[10];
    void *q;
    int num;

    for (i = 0; i < 10; i++) {
        if (i % 2 == 0)
            pointers[i] = worst_fit_alloc(31);
        else
            pointers[i] = worst_fit_alloc(6);
        printf("worst fit: p=%p\n", pointers[i]);
    }

    if (pointers[1] != NULL) // Dealloc 8 bytes
        worst_fit_dealloc(pointers[1]);

    if (pointers[2] != NULL) // Dealloc 32 bytes
        worst_fit_dealloc(pointers[2]);

    if (pointers[5] != NULL) // Dealloc 8 bytes
        worst_fit_dealloc(pointers[5]);

    q = worst_fit_alloc(4);

    num = worst_fit_count_extfrag(69);
    printf("Ext frag: %d\n", num);

    return 0;
}

int comparison_test() {
    int i;
    best_fit_memory_init(128);

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

    num = best_fit_count_extfrag(69);
    printf("Best Fit Ext frag: %d\n", num);

    
    
    worst_fit_memory_init(128);

    for (i = 0; i < 10; i++) {
        if (i % 2 == 0)
            pointers[i] = worst_fit_alloc(32);
        else
            pointers[i] = worst_fit_alloc(8);
        printf("worst fit: p=%p\n", pointers[i]);
    }

    if (pointers[1] != NULL) // Dealloc 8 bytes
        worst_fit_dealloc(pointers[1]);

    if (pointers[2] != NULL) // Dealloc 32 bytes
        worst_fit_dealloc(pointers[2]);

    if (pointers[5] != NULL) // Dealloc 8 bytes
        worst_fit_dealloc(pointers[5]);

    q = worst_fit_alloc(4);

    num = worst_fit_count_extfrag(69);
    printf("Worst Fit Ext frag: %d\n", num);


    printf("\n-----------------------------------------------------------\n");

    print_bitmaps();

    return 0;
}
