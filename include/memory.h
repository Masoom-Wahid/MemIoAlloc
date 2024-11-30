#ifndef memory_io_h
#define memory_io_h

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>


// Variables
#define MEM_IO_PAGE 4096
#define MEM_IO_HEADER_SIZE 12
#define MEM_IO_ON_OFFSET 0
#define MEM_IO_SIZE_OFFSET 4
#define MEM_IO_USED_OFFSET 8
#define MEM_IO_ON 1
#define MEM_IO_OFF 0


#define MEM_IO_FLAG_FREE 0x01
#define MEM_IO_FLAG_USED 0x02
#define MEM_IO_FLAG_CORRUPTED 0x04
#define MEM_IO_MAGIC_NUMBER 0xDEADBEEF


/*
 *
 */
typedef struct {
	size_t size;        // Size of the block in bytes
	size_t *next;       // Pointer to the next block
	size_t *prev;       // Pointer to the previous block
	/*
	 * if the flag is free or if it is used and free or if it is free and not used etc...
	 * since we are using 8bits we have 255 flags , only 3 valid tho
	*/
	uint8_t flags;       

	uint32_t magic;
} mem_io_block;




typedef struct {
	bool is_init; // have we initilized our memeory yet or not
	mem_io_block *head; // to keep the original pointer to our malloc , to search for a place later
	size_t *heap; // curr heap pointer
	size_t *seg_break; // curr break segment pointer
	size_t count; // the amount of object collected
	// TODO: implement 'used_queue'
	//uint8_t *used_queue;
	// DEBUG PURPOSES
	uint8_t allocated; // the amount of objects alloacted
	uint8_t deallocated; // the amount of objects deallocated
	size_t total_size;
	size_t free_size;
	uint8_t largest_free_block;
	uint8_t errors;
} memory_io;



void* mem_io_malloc(); 
void mem_io_free(void *addr); 
void* mem_io_realloc(void *_addr,size_t size);
void* mem_io_calloc();
void mem_io_log_trace();
void mem_io_stack_trace();
// Macros

#define IS_MEM_IO_FLAG_FREE(flags) ((flags) & MEM_IO_FLAG_FREE)
#define IS_MEM_IO_FLAG_USED(flags) ((flags) & MEM_IO_FLAG_USED)

#define IS_MEM_IO_FLAG_CORRUPTED(flags) ((flags) & MEM_IO_FLAG_CORRUPTED)

#define VALIDATE_BLOCK(block) ((block) && ((block)->magic == MEM_IO_MAGIC_NUMBER))


#define ADVANCE_HEAP_POINTER(heap, block_size) \
    (heap = (size_t *)((uint8_t *)(heap) + sizeof(mem_io_block) + (block_size)))

#define ALIGNMENT sizeof(void*)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))


/*
 * UNIX function defintions
*/
void* sbrk(intptr_t increment);
int brk(void *addr);

#endif
