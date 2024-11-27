#ifndef memory_io_h
#define memory_io_h

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <stdlib.h>

// Variables
#define MEM_IO_PAGE 4096
#define MEM_IO_HEADER_SIZE 12
#define MEM_IO_ON_OFFSET 0
#define MEM_IO_SIZE_OFFSET 4
#define MEM_IO_USED_OFFSET 8
#define MEM_IO_ON 1
#define MEM_IO_OFF 0



typedef struct {
	bool is_init; // have we initilized our memeory yet or not
	uint8_t *head; // to keep the original pointer to our malloc , to search for a place later
	uint8_t *heap; // curr heap pointer
	uint8_t *seg_break; // curr break segment pointer
	size_t count; // the amount of object collected
	// DEBUG PURPOSES
	size_t allocated; // the amount of objects alloacted
	size_t deallocated; // the amount of objects deallocated
} memory_io;



void* mem_io_malloc(); 
void mem_io_free(void *addr); 
void* mem_io_realloc(void *addr);
void* mem_io_calloc();
void mem_io_log_trace();

// Macros


#define ALIGNMENT sizeof(void*)
#define ALIGN(size) (((size) + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1))


/*
 * UNIX function defintions
*/
void* sbrk(intptr_t increment);
int brk(void *addr);

#endif
