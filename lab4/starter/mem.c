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
#include <limits.h>

/* defines */
#define BLOCK_SIZE 4

/* global varaiables */
void* b_mem_box;
void* b_free_space;
int* b_bitmap;
int M;
int N;

void* w_mem_box;
void* w_free_space;
int* w_bitmap;

/* Bitmap Functions */

void  SetBit( int A[],  int k )
   {
      A[k/32] |= 1 << (k%32);  // Set the bit at the k-th position in A[i]
   }

void  ClearBit( int A[],  int k )                
   {
      A[k/32] &= ~(1 << (k%32));
   }

int TestBit( int A[],  int k )
   {
      return ( (A[k/32] & (1 << (k%32) )) != 0 ) ;     
   }

/* memory initializer */
int best_fit_memory_init(size_t size)
{

	if (size < BLOCK_SIZE) {
	    return -1;
    }
    
    printf("initializing memory of size %d\n", (int) size);

	b_mem_box = malloc(size);
	b_bitmap = (int*) b_mem_box;

	// M blocks i.e. M numbers to keep track of
	M = size/BLOCK_SIZE;
	if (size % BLOCK_SIZE != 0) M++;

    // M numbers takes x # of bits to represent
    int x = ceil(log2f(M));

    printf("M: %d, x: %d\n", M, x);

	int i;
	for (i = 0; i < M; i++) {
	   if (i < x ) { 
	       SetBit(b_bitmap, i);
       } else {
           ClearBit(b_bitmap, i);
       }
    }

    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");
	
	b_free_space = b_mem_box + x; 
	// printf("bitmap address: %d, bitmap_size: %d, free_space address: %d\n", b_mem_box, x, b_free_space);

	return 0;

}

int worst_fit_memory_init(size_t size)
{

    if (size < BLOCK_SIZE) {
	    return -1;
    }
    
    printf("initializing memory of size %d\n", (int) size);

	w_mem_box = malloc(size);
	w_bitmap = (int*) w_mem_box;

	// N blocks i.e. N numbers to keep track of
	N = size/BLOCK_SIZE;
	if (size % BLOCK_SIZE != 0) N++;

    // N numbers takes x # of bits to represent
    int x = ceil(log2f(N));

    printf("N: %d, x: %d\n", N, x);

	int i;
	for (i = 0; i < N; i++) {
	   if (i < x ) { 
	       SetBit(w_bitmap, i);
       } else {
           ClearBit(w_bitmap, i);
       }
    }

    for (i = 0; i < N; i++) {
        printf("%d", TestBit(w_bitmap,i));
    }
    printf("\n");
	
	w_free_space = w_mem_box + x; 
	// printf("bitmap address: %d, bitmap_size: %d, free_space address: %d\n", w_mem_box, x, w_free_space);

	return 0;
}


/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	// each allocation will require 1 block before it to store the number of blocks allocated
	// all of the blocks should be stored as 1 in the bitmap
   
    printf("Best fit alloc of size %d\n", (int) size);

    if (size == 0) {
        return NULL;
    }
    
    int required_blocks = size/BLOCK_SIZE;
    // if there is a remainder need an extra block
    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }
    int best_fit_start = 0;
    int best_fit_size = INT_MAX;
    int start;
    int in_contiguous = 0;
    int contiguous_size = 0;
    
    int i;
    //for (i = 0; i < M; i++) {
    //    printf("%d", TestBit(b_bitmap,i));
    //}
    //printf("\n");
    
    // Finding the best fit start and size
    // i.e. the smallest contiguous set of 0s bigger than required_blocks+1
    for (i = 0; i < M; i++) {
        //printf("Bit %d: %d, in_contiguous: %d, contiguous_size: %d\n",i, TestBit(b_bitmap,i), in_contiguous, contiguous_size);

        // entering a contiguous section
        if (TestBit(b_bitmap,i) == 0 && !in_contiguous) {
            in_contiguous = 1;
            start = i;
            //printf("%d: entering contiguous section\n", i);
        }

        // in a contiguous section
        if (TestBit(b_bitmap,i) == 0 && in_contiguous) {
            contiguous_size++;
            //printf("%d: in contiguous section\n", i);
        }

        // leaving a contiguous section
        if ((TestBit(b_bitmap,i) == 1 && in_contiguous) || i == M-1) {
            in_contiguous = 0;
            // printf("%d: left contiguous section, required_blocks: %d, contiguous_size: %d, best_fit_size: %d\n", i, required_blocks, contiguous_size, best_fit_size);
            if (contiguous_size - required_blocks > 0 && contiguous_size - required_blocks < best_fit_size - required_blocks) {
                // printf("new best fit found. contiguous size: %d\n", contiguous_size); 
                best_fit_size = contiguous_size;
                best_fit_start = start;
                contiguous_size = 0;
            }
        }

        // else have only seen 1s so far so just iterate
    }

    if (best_fit_size > required_blocks + 1) {
        best_fit_size = required_blocks + 1;
    }

    // printf("best_fit_start: %d, best_fit_size: %d\n",best_fit_start, best_fit_size); 

    // couldn't find any contiguous block big enough
    if (best_fit_size == INT_MAX) {
        printf("Returning null\n");
        return NULL;
    }

    // Mark all the blocks as used in bitmap
    for (i = best_fit_start; i < best_fit_start + best_fit_size; i++) {
        SetBit(b_bitmap,i);
    }

    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");

    // Mark first block of best_fit with size
    void *temp = b_free_space + (best_fit_start)*BLOCK_SIZE;
    int* size_block = (int *) temp;
    *size_block = best_fit_size;
 
	return b_free_space + (best_fit_start+1)*BLOCK_SIZE;
}


void *worst_fit_alloc(size_t size)
{
    int worst_fit_start = 0, start = 0, in_contiguous = 0, contiguous_size = 0, worst_fit_size = 0;

    printf("Worst fit alloc of size %d\n", (int) size);

    int required_blocks = size/BLOCK_SIZE;
    // if there is a remainder need an extra block
    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }

    if (size == 0) return NULL;

    // Find the worst fit start and size
    // i.e. Find the largest contiguous set of 0s
    int i;
    for (i = 0; i < N; i++) {
        //printf("Bit %d: %d, in_contiguous: %d, contiguous_size: %d\n",i, TestBit(w_bitmap,i), in_contiguous, contiguous_size);

        // entering a contiguous section
        if (TestBit(w_bitmap,i) == 0 && !in_contiguous) {
            in_contiguous = 1;
            start = i;
            // printf("%d: entering contiguous section\n", i);
        }

        // in a contiguous section
        if (TestBit(w_bitmap,i) == 0 && in_contiguous) {
            contiguous_size++;
            //printf("%d: in contiguous section\n", i);
        }

        // leaving a contiguous section
        if ((TestBit(w_bitmap,i) == 1 && in_contiguous) || i == N-1) {
            // printf("%d: left contiguous section, contiguous_size: %d, worst_fit_size: %d\n", i, contiguous_size, worst_fit_size);
            in_contiguous = 0;
            if (contiguous_size > worst_fit_size && contiguous_size > required_blocks) {
                worst_fit_size = contiguous_size;
                worst_fit_start = start;
            }
            contiguous_size = 0;
        }
    }

    if (worst_fit_size > required_blocks + 1) {
        worst_fit_size = required_blocks + 1;
    }

    // printf("worst_fit_start: %d, worst_fit_size: %d\n",worst_fit_start, worst_fit_size);

    // No free space or free space is less than number of blocks required
    if (worst_fit_size == 0) return NULL;

    // Mark all the blocks as used in bitmap
    for (i = worst_fit_start; i < worst_fit_start + worst_fit_size; i++) {
        SetBit(w_bitmap,i);
    }

    for (i = 0; i < N; i++) {
        printf("%d", TestBit(w_bitmap,i));
    }
    printf("\n");

    // Mark first block of best_fit with size
    void *temp = w_free_space + (worst_fit_start)*BLOCK_SIZE;
    int* size_block = (int *) temp;
    *size_block = worst_fit_size;

    return w_free_space + (worst_fit_start+1)*BLOCK_SIZE;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{

    if (ptr == NULL) return;

    int* user_mem_start = (int*) ptr;
    int* block_mem_start = user_mem_start - 1;
    int block_size = *block_mem_start;

    printf("Dealloc size = %d\n", block_size);
	
	int block_index = block_mem_start - (int*) b_free_space;
	// printf("Block index = %d\n", block_index);

    int i;
    // Mark all the blocks as free in bitmap
    for (i = block_index; i < block_index + block_size; i++) {
        ClearBit(b_bitmap,i);
    }
    
    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");

	return;
}

void worst_fit_dealloc(void *ptr) 
{
    
    if (ptr == NULL) return;

    int* user_mem_start = (int*) ptr;
    int* block_mem_start = user_mem_start - 1;
    int block_size = *block_mem_start;

    printf("Dealloc size = %d\n", block_size);

    int block_index = block_mem_start - (int*) w_free_space;
    // printf("Block index = %d\n", block_index);

    int i;
    // Mark all the blocks as free in bitmap
    for (i = block_index; i < block_index + block_size; i++) {
        ClearBit(w_bitmap,i);
    }

    for (i = 0; i < N; i++) {
        printf("%d", TestBit(w_bitmap,i));
    }
    printf("\n");

    return;
}

/* memory algorithm metric utility function(s) */

/* count how many free blocks are less than the input size */ 
int best_fit_count_extfrag(size_t size)
{
    if (size == 0) {
        return -1;
    }
    
    int required_blocks = size/BLOCK_SIZE;
    // if there is a remainder need an extra block
    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }
    int in_contiguous = 0;
    int contiguous_size = 0;
    int num_fits = 0;

    int i;
    for (i = 0; i < M; i++) {

        // entering a contiguous section
        if (TestBit(b_bitmap,i) == 0 && !in_contiguous) {
            // printf("%d: entering contiguous section\n", i);
            in_contiguous = 1;
        }

        // in a contiguous section
        if (TestBit(b_bitmap,i) == 0 && in_contiguous) {
            // printf("%d: in contiguous section\n", i);
            contiguous_size++;
        }

        // leaving a contiguous section
        if ((TestBit(b_bitmap,i) == 1 || i == M-1) && in_contiguous) {
            // printf("%d: left contiguous section, contiguous_size: %d, required_blocks: %d\n", i, contiguous_size, required_blocks);
            in_contiguous = 0;
            if (contiguous_size < required_blocks + 1) {
                num_fits++;
            }
            contiguous_size = 0;
        }

        // else have only seen 1s so far so just iterate
    }
	return num_fits;
}

int worst_fit_count_extfrag(size_t size)
{
    if (size == 0) return -1;

    int required_blocks = size/BLOCK_SIZE;
    // if there is a remainder need an extra block
    if (size % BLOCK_SIZE != 0) {
        required_blocks++;
    }
    int in_contiguous = 0;
    int contiguous_size = 0;
    int num_fits = 0;

    int i;
    for (i = 0; i < N; i++) {

        // entering a contiguous section
        if (TestBit(w_bitmap,i) == 0 && !in_contiguous)
            in_contiguous = 1;

        // in a contiguous section
        if (TestBit(w_bitmap,i) == 0 && in_contiguous)
            contiguous_size++;

        // leaving a contiguous section
        if ((TestBit(w_bitmap,i) == 1 || i == N-1) && in_contiguous) {
            in_contiguous = 0;
            if (contiguous_size < required_blocks + 1) {
                num_fits++;
                contiguous_size = 0;
            }
        }
    }
    return num_fits;
}


void print_bitmaps() {
    
    int i;
    printf("Best Fit Bitmap: \n");
    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");
    
    printf("Worst Fit Bitmap: \n");
    for (i = 0; i < N; i++) {
        printf("%d", TestBit(w_bitmap,i));
    }
    printf("\n");
}
