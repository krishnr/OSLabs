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

void* w_mem_box;
void* w_free_space;
int* w_bitmap;

/* Functions */

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

	if (size < 4) {
	    return -1;
    }

	b_mem_box = malloc(size);
	b_bitmap = (int*) b_mem_box;

	// M number of blocks
	M = size/BLOCK_SIZE;

    int n = BLOCK_SIZE * 8;

    // size in bytes
    int bitmap_size = (int) (M + (n - M%n));

    printf("Bitmap size: %d, size: %zu, bitmap: %d\n", bitmap_size, size, b_bitmap);

	int i;
	for (i = 0; i < M; i++) {
	   if (i < bitmap_size/n) {
	       SetBit(b_bitmap, i);
       } else {
           ClearBit(b_bitmap, i);
       }
    }

    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");
	
	b_free_space = b_mem_box + bitmap_size;
	printf("bitmap address: %d, bitmap_size: %d, free_space address: %d\n", b_mem_box, bitmap_size, b_free_space);

	return 0;

}

int worst_fit_memory_init(size_t size)
{
	return 0;
}


/* memory allocators */
void *best_fit_alloc(size_t size)
{
	// To be completed by students
	// each allocation will require 1 block before it to store the number of blocks allocated
	// all of the blocks should be stored as 1 in the bitmap
    
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
    for (i = 0; i < M; i++) {
        printf("%d", TestBit(b_bitmap,i));
    }
    printf("\n");
    
    // Finding the best fit start and size
    // i.e. the smallest contiguous set of 0s bigger than required_blocks+1
    for (i = 0; i < M; i++) {
        printf("Bit %d: %d, in_contiguous: %d, contiguous_size: %d\n",i, TestBit(b_bitmap,i), in_contiguous, contiguous_size);

        // entering a contiguous section
        if (TestBit(b_bitmap,i) == 0 && !in_contiguous) {
            in_contiguous = 1;
            start = i;
            printf("%d: entering contiguous section\n", i);
        }

        // in a contiguous section
        if (TestBit(b_bitmap,i) == 0 && in_contiguous) {
            contiguous_size++;
            printf("%d: in contiguous section\n", i);
        }

        // leaving a contiguous section
        if ((TestBit(b_bitmap,i) == 1 && in_contiguous) || i == M-1) {
            in_contiguous = 0;
            printf("%d: left contiguous section, required_blocks: %d, contiguous_size: %d, best_fit_size: %d\n", i, required_blocks, contiguous_size, best_fit_size);
            if (contiguous_size - required_blocks > 0 && abs(contiguous_size - required_blocks) < abs(best_fit_size - required_blocks)) {
                if (contiguous_size > required_blocks + 1) {
                    contiguous_size = required_blocks + 1;
                }
                best_fit_size = contiguous_size;
                best_fit_start = start;
            }
        }

        // else have only seen 1s so far so just iterate
    }

    printf("best_fit_start: %d, best_fit_size: %d\n",best_fit_start, best_fit_size); 

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
	// To be completed by students
	return NULL;
}

/* memory de-allocator */
void best_fit_dealloc(void *ptr) 
{
    int* user_mem_start = (int*) ptr;
    int* block_mem_start = user_mem_start - 1;
    int block_size = *block_mem_start;

    printf("Dealloc size = %d\n", block_size);
	
	int block_index = block_mem_start - (int*) b_free_space;
	printf("Block index = %d\n", block_index);

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
