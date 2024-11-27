#include <unistd.h>
#include "memory.h"

memory_io mem_io;


void init_mem_io(){
	mem_io.is_init=true;
	int* curr_break = (int*)sbrk(0);
	//printf("memory->curr_break %p\n",curr_break); 
	mem_io.curr_break = curr_break+1;
	mem_io.heap = curr_break+1;
}

/*
* mem_io's internal alloc , allocates memeory and adjusts the size 
* of heap and curr_break
* for now we just adjust the break with 1 page ( 4096 bytes) maybe this would be better
* to adjust to a page ??
*/
void __mem_io_alloc(){
	int* new_break = sbrk(MEM_IO_PAGE*4);
	mem_io.heap = new_break;
}


void* mem_io_malloc(size_t size){
	if (mem_io.is_init == false){
		init_mem_io();
	}

	//printf("memory->is_init %b\n",mem_io.is_init); 
	printf("START: memory->curr_break %p\n",mem_io.curr_break); 
	printf("START: memory->heap %p\n",mem_io.heap); 

	/*
	 * if heap and curr_break is the same
	 * we need more memory
	*/
	if (mem_io.curr_break == mem_io.heap){
		__mem_io_alloc();
	}
	

	for(;;){
		if(*(mem_io.curr_break) == MEM_IO_ON) continue;
		*(mem_io.curr_break + MEM_IO_ON_OFFSET) = MEM_IO_ON;
		*(mem_io.curr_break + MEM_IO_SIZE_OFFSET) = size;
		printf("FR FR 1 memory->curr_break %p\n",mem_io.curr_break); 
		mem_io.curr_break += MEM_IO_HEADER_SIZE;
		printf("FR FR 2 memory->curr_break %p\n",mem_io.curr_break); 
		break;
	}

	void* return_pointer = (void*)mem_io.curr_break;
	mem_io.curr_break += size;
	//printf("memory->is_init %b\n",mem_io.is_init); 
	printf("END: memory->curr_break %p\n",mem_io.curr_break); 
	printf("END: memory->heap %p\n",mem_io.heap); 
	printf("END: returning -> %p\n\n\n\n\n",return_pointer);

	return return_pointer;
}



void mem_io_free(void* _addr){
	int* addr = (int*)_addr;
	printf("addr -> %p\n",addr-8);
	if(*(addr - MEM_IO_ON_OFFSET) == MEM_IO_OFF){
		return;
	}

	*(addr- MEM_IO_ON_OFFSET) = MEM_IO_OFF;
	return;
}


