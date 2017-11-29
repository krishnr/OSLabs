/**
 * @file memory.c
 * @brief: ECE254 Lab: memory allocation algorithm comparison template file
 * @author: Krishn Ramesh and Ramandeep Farmaha 
 * @date: 2017-11-29
 */

/* includes */
#include <stdio.h> 
#include <stdlib.h> 
#include "mem.h"
#include <math.h>

/* defines */
#define BLOCK_SIZE 4
#define SetBit(A,k)     ( A[(k/32)] |= (1 << (k%32)) )
#define ClearBit(A,k)   ( A[(k/32)] &= ~(1 << (k%32)) )
#define TestBit(A,k)    ( A[(k/32)] & (1 << (k%32)) )

/* global varaiables */
void* b_mem_box;
void* b_free_space;
int* b_bitmap;

void* w_mem_box;
void* w_free_space;
int* w_bitmap;

/* Functions */

/* memory initializer */
int best_fit_memory_init(size_t size)
{

	if (size < 4) {
	    return -1;
    }

	b_mem_box = malloc(size);
	b_bitmap = (int*) b_mem_box;

	// M number of blocks
	int M = size/BLOCK_SIZE;
	
    int bitmap_size = (int) ceil(M/8);

    printf("Bitmap size: %d, size: %zu, bitmap: %d\n", bitmap_size, size, b_bitmap);

	int i;
	for (i = 0; i < M; i++) {
	   if (i < bitmap_size) {
	       SetBit(b_bitmap, i);
       } else {
           ClearBit(b_bitmap, i);
       }
    }

    if ( TestBit(b_bitmap, 10) )
         printf("Bit %d was set !\n", 10);
    if ( TestBit(b_bitmap, 31) )
         printf("Bit %d was set !\n", 31);
    printf("Should be 0: %d %d\n", TestBit(b_bitmap, 32), TestBit(b_bitmap, 255));
	
	b_free_space = b_mem_box + bitmap_size;
	printf("bitmap address: %d, bitmap_size: %d, free_space address: %d\n", b_mem_box, bitmap_size, b_free_space);

	return 0;

}

int worst_fit_memory_init(size_t size)
{
	if (size < 4) {
	    return -1;
    }

	w_mem_box = malloc(size);
	w_bitmap = (int*) w_mem_box;

	// M number of blocks
	int M = size/BLOCK_SIZE;
	
    int bitmap_size = (int) ceil(M/8);

    printf("Bitmap size: %d, size: %zu, bitmap: %d\n", bitmap_size, size, w_bitmap);

	int i;
	for (i = 0; i < M; i++) {
	   if (i < bitmap_size) {
	       SetBit(w_bitmap, i);
       } else {
           ClearBit(w_bitmap, i);
       }
    }

    if ( TestBit(w_bitmap, 10) )
         printf("Bit %d was set !\n", 10);
    if ( TestBit(w_bitmap, 31) )
         printf("Bit %d was set !\n", 31);
    printf("Should be 0: %d %d\n", TestBit(w_bitmap, 32), TestBit(w_bitmap, 255));
	
	w_free_space = w_mem_box + bitmap_size;
	printf("bitmap address: %d, bitmap_size: %d, free_space address: %d\n", w_mem_box, bitmap_size, w_free_space);

	return 0;

}


/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	return NULL;
}


void *worst_fit_alloc(size_t size)
{
	// To be completed by students
	return NULL;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{

	// To be completed by students
	return;
}

void worst_fit_dealloc(void *ptr) 
{

	// To be completed by students
	return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}

int worst_fit_count_extfrag(size_t size)
{
	// To be completed by students
	return 0;
}
