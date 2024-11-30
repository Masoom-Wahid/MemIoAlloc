#ifndef memory_io_h
#define memory_io_h

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

// Variables
#define MEM_IO_PAGE 4096
#define MEM_IO_FLAG_FREE 0x01
#define MEM_IO_FLAG_USED 0x02
#define MEM_IO_FLAG_CORRUPTED 0x04
#define MEM_IO_FLAG_MMAP 0x08
#define MEM_IO_MAGIC_NUMBER 0xDEADBEEF
#define MINIMUM_ALLOC_SIZE 16 
#define MEM_IO_MMAP_THRESHOLD (1024*128) // 128 kb or bigger we use mmap



/*
 *
 *
High Memory Addresses (e.g., 0x7FFFFFFF)
+-----------------------+  
|       Stack           |   <- Grows downward  
|       (local vars)    |
|       ...             |
|                       |
+-----------------------+  
|      Unused Space     |  
+-----------------------+ heap_end  -> (beginning of managed heap) 
|       Heap            | 
|       (malloc'd data) | <- Grows upward 
|       ...             |
+-----------------------+ heap_start    -> (current end of the managed heap) 
|       BSS Segment     |  
+-----------------------+  
|       Data Segment    |  
+-----------------------+  
|       Text Segment    |  
+-----------------------+  
Low Memory Addresses (e.g., 0x00000000)

 */
typedef struct {
	size_t size;        // Size of the block in bytes
	uint8_t *next;       // Pointer to the next block
	uint8_t *prev;       // Pointer to the previous block
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
	uint8_t *heap_start; // curr heap pointer
	uint8_t *heap_end; // curr heap_end 
	size_t count; // the amount of object collected
	// TODO: implement 'used_queue'
	//uint8_t *used_queue;
	// DEBUG PURPOSES
	size_t allocated; // the amount of objects alloacted
	size_t deallocated; // the amount of objects deallocated
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
#define IS_MEM_IO_FLAG_MMAP(flags) ((flags) & MEM_IO_FLAG_MMAP)

#define VALIDATE_BLOCK(block) ((block) && ((block)->magic == MEM_IO_MAGIC_NUMBER))


#define ADVANCE_HEAP_POINTER(heap, block_size) \
    (heap = (uint8_t *)(heap) + sizeof(mem_io_block) + (block_size))

#define ALIGNMENT sizeof(void*)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))


/*
 * UNIX function defintions
*/
void* sbrk(intptr_t increment);
int brk(void *addr);
void *mmap(void *addr, size_t len, int prot, int flags,int fildes, off_t off);



#endif
