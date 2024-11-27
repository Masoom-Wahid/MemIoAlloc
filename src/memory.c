#include <unistd.h>
#include "memory.h"


/*
 * mem_io represents the base structure for our malloc.
 * it has head,heap,seg_break
 * head -> the head of the heap
 * heap -> the curr pointer to the strcthed heap
 * seg_break -> curr pointer to the break segment 
 * we also have additional infos like count,alloacted,deallocated
 *
 * 
*/
memory_io mem_io;



/*
 * using just a basing mutex lock and unlock to ensure thread saftery (as if that really matters)
 * 
 */
pthread_mutex_t mem_lock = PTHREAD_MUTEX_INITIALIZER;





void init_mem_io(){
	mem_io.is_init=true;
	uint8_t* curr_break = (uint8_t*)sbrk(0);
	mem_io.seg_break = curr_break+1;
	mem_io.heap = curr_break+1;
	mem_io.head = curr_break+1;
	mem_io.count = 0;
	mem_io.allocated = 0;
	mem_io.deallocated = 0;
}



/*
* mem_io's internal alloc , allocates memeory and adjusts the size 
* of heap and curr_break
* for now we just adjust the break with 1 page ( 4096 bytes) maybe this would be better
* to adjust to a page ??
*/
void __mem_io_alloc(size_t size){
	uint8_t* new_break = (uint8_t*)sbrk(size*4);// just to make sure not to syscall everytime
	if (new_break == (void*) -1) {
		perror("sbrk failed");
		exit(EXIT_FAILURE);
	    }
	mem_io.heap = new_break;
}



/*
 * update the ptr to next location
 */
uint8_t *update_ptr(uint8_t *ptr){
	size_t __size = (size_t)*(ptr + MEM_IO_SIZE_OFFSET);
	uint8_t *new_ptr = ptr + MEM_IO_HEADER_SIZE + __size;
	return new_ptr;
}

void* mem_io_malloc(size_t size){
	pthread_mutex_lock(&mem_lock);
	if (mem_io.is_init == false){
		init_mem_io();
	}

	/*
	printf("memory->is_init %b\n",mem_io.is_init); 
	printf("START: memory->break %p\n",mem_io.seg_break); 
	printf("START: memory->heap %p\n",mem_io.heap); 
	*/


	/*
	 * if heap and curr_break is the same
	 * we need more memory
	*/

	uint8_t *curr_ptr = mem_io.head;


	while(1){
		// if we have reached the end of our journey , we shall expand , aint no way we stopping
		if(curr_ptr == mem_io.seg_break)  __mem_io_alloc(size);

		//printf("\n\nHELL NAH is %p\n\n",curr_ptr);

		if(*(curr_ptr) == MEM_IO_ON) {
			//printf("PTR %p\n",curr_ptr);
			//size_t __size = (size_t)*(curr_ptr + MEM_IO_SIZE_OFFSET);
			//printf("size is %ld\n",__size);
			curr_ptr = update_ptr(curr_ptr);
			//printf("UPDATE_PTR %p\n",curr_ptr);
			continue;
		}


		if(*(curr_ptr + MEM_IO_USED_OFFSET) == MEM_IO_ON){
			bool is_enough_size = *(curr_ptr + MEM_IO_SIZE_OFFSET) >= size;
			//printf("is_enough_size %d\n",is_enough_size);
			if (!is_enough_size) {
				curr_ptr = update_ptr(curr_ptr);
				continue;
			} 
		}


		*(curr_ptr + MEM_IO_ON_OFFSET) = MEM_IO_ON;
		*(curr_ptr + MEM_IO_SIZE_OFFSET) = size;
		*(curr_ptr + MEM_IO_USED_OFFSET) = MEM_IO_ON;
		

		//printf("FR FR 1 memory->curr_break %p\n",mem_io.seg_break); 
		curr_ptr += MEM_IO_HEADER_SIZE;
		

		//printf("FR FR 2 memory->curr_break %p\n",mem_io.seg_break); 
		
		break;
	}

	void* return_pointer = (void*)curr_ptr;
	//mem_io.curr_break += size;
	mem_io.allocated += 1;
	mem_io.count += 1;
	

	/*
	printf("memory->is_init %b\n",mem_io.is_init); 
	printf("END: memory->curr_break %p\n",mem_io.seg_break); 
	printf("END: memory->heap %p\n",mem_io.heap); 
	*/
	//printf("\nEND: returning -> %p\n",return_pointer);


	pthread_mutex_unlock(&mem_lock);
	return return_pointer;
}



void mem_io_free(void* _addr){
	pthread_mutex_lock(&mem_lock);
	uint8_t* addr = (uint8_t*)_addr;

	if(*(addr - MEM_IO_HEADER_SIZE) == MEM_IO_OFF){
		pthread_mutex_unlock(&mem_lock);
		return;
	}

	*(addr- MEM_IO_HEADER_SIZE) = MEM_IO_OFF;
	mem_io.count -= 1;
	mem_io.deallocated += 1;
	pthread_mutex_unlock(&mem_lock);

	return;
}

void mem_io_log_trace(){
	printf(
	"\n\n\nDEBUG INFO:\n\nseg_break: %p\nheap: %p\nhead: %p\ncount: %ld\nallocated: %ld\ndeallocated: %ld\n\n\n",
	mem_io.seg_break,
	mem_io.heap,
	mem_io.head,
	mem_io.count,
	mem_io.allocated,
	mem_io.deallocated
	);
}
