#ifndef memory_io_h
#define memory_io_h

#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

// Variables
#define MEM_IO_PAGE 4096
#define MEM_IO_HEADER_SIZE 8
#define MEM_IO_ON_OFFSET 0
#define MEM_IO_SIZE_OFFSET 4
#define MEM_IO_ON 1
#define MEM_IO_OFF 0



typedef struct {
	bool is_init; // have we initilized our memeory yet or not
	int *heap; // curr heap pointer
	int *curr_break; // curr break segment pointer
} memory_io;




void* mem_io_malloc(); 
void mem_io_free(void *addr); 

/*
 * UNIX function defintions
*/
void* sbrk(intptr_t increment);
int brk(void *addr);

#endif
