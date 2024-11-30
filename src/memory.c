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



/*
* mem_io's internal alloc , allocates memeory and adjusts the size 
* of heap and curr_break
* for now we just adjust the break with 1 page ( 4096 bytes) maybe this would be better
* to adjust to a page ??
*/
void __mem_io_alloc(size_t size){
	size_t allocation_size = ALIGN(size + sizeof(mem_io_block)); 
	size_t *new_break = (size_t*)sbrk(allocation_size); 
	size_t size_diff = (size_t)(new_break - mem_io.heap);
	if (new_break == (void*) -1) {
		perror("sbrk failed");
		exit(EXIT_FAILURE);
	}
	mem_io.total_size += size_diff; 
	mem_io.heap = new_break;
}



/*
* __mem_io_init_pool initilize the pool by setting everything up and then
* it allocate a PAGE of memory
*/
void __mem_io_init_pool(){
	mem_io.is_init=true;
	size_t* curr_break = (size_t*)sbrk(0);
	
	mem_io.seg_break = curr_break+1;
	mem_io.heap = curr_break+1;
	mem_io.head = NULL;
	mem_io.total_size = 0;
	mem_io.free_size = 0;
	mem_io.count = 0;
	mem_io.largest_free_block = 0;
	mem_io.errors = 0;
	mem_io.allocated = 0;
	mem_io.deallocated = 0;
	
	__mem_io_alloc(MEM_IO_PAGE);

}



void __init_block(mem_io_block *block,size_t* next,size_t *prev,size_t size){
	block->size = size;
	block->next = next; 
	block->prev = prev;
	block->flags = MEM_IO_FLAG_USED;
	block->magic = MEM_IO_MAGIC_NUMBER;

}



/*
* when given 2 blocks to split , first find the area we want to split upon 
* so if the main block is of 235 size and we have a requester of 90 that means
* 90+32 (32 being the block size as of now) so 122 , so the next block would start upon
* 123 until 235. which again 123+32=155 so the new block data starts at 155
* this function is usefull for not wasting memeory , if we have a hugepool and the user requests a little
* we can give that particular area for the user
*
*/
void __split_block(mem_io_block *block,size_t size){
	mem_io_block *new_block = (mem_io_block*)((uint8_t*)block + sizeof(mem_io_block) + size);
	__init_block(
		new_block,
		block->next,
		(size_t*)block,
		(block->size-size)-sizeof(mem_io_block)
	);
	block->next = (size_t*)new_block;
	block->size = size;
	new_block->flags = MEM_IO_FLAG_FREE;
}


/*
 * simply just merges the size of blocks and then adjusts the pointers
*/ 
void __merge_blocks(mem_io_block* block_1,mem_io_block *block_2){
	size_t block_total_size = block_1->size+sizeof(mem_io_block)+block_2->size;
	block_1->next = block_2->next;
	block_1->size = block_total_size;
}


void block_debug(mem_io_block *block) {
    if (block == NULL) {
        printf("Error: Block is NULL.\n");
        return;
    }

    printf(
        "\n\n\n=== Block Debug Info ===\n"
        "Block Address: %p\n"
        "Size: %zu bytes\n"
        "Next Block: %p\n"
        "Previous Block: %p\n"
        "Flags: %s%s%s\n"
        "Magic: 0x%08x %s\n"
        "=========================\n\n\n",
        (void *)block,                      
        block->size,                       
        (void *)block->next,                
        (void *)block->prev,                
        (block->flags & MEM_IO_FLAG_FREE) ? "Free " : "",
        (block->flags & MEM_IO_FLAG_USED) ? "Used " : "",
        (block->flags & MEM_IO_FLAG_CORRUPTED) ? "Corrupted " : "",
        block->magic,                       
        (block->magic != 0xDEADBEEF) ? "(Warning: Possible corruption)" : ""
    );
}


/*
* the main function of mem_io
* the whole idea is :
* first check if we have a free area
* if so then just check if that area is not too big , if it is so we call __split_block
* else we just take this area for the user
* otherwise if there aint no area available we take a new one but also adjust the heap size
* if somehow we have reached the end of heap and there aint no area available
* to take for ourselves we shall feast on the new memory given to us by kernel
*/
void* mem_io_malloc(size_t size){
	pthread_mutex_lock(&mem_lock);
	if (!mem_io.is_init) __mem_io_init_pool();

	mem_io_block *curr_ptr = mem_io.head;
	mem_io_block *prev_ptr = mem_io.head;
	bool is_head_null = curr_ptr == NULL;


	while(1){
		// if we have reached the end of our journey , we shall expand , aint no way we stopping
		if((size_t *)curr_ptr == mem_io.seg_break)  __mem_io_alloc(size);


		if (curr_ptr == NULL){
			mem_io_block *new_block = (mem_io_block *)mem_io.heap;
			__init_block(new_block,NULL,NULL,size);
			

			if(is_head_null){ 
				mem_io.head = new_block;
			}else{
				new_block->prev = (size_t*)prev_ptr; 
				prev_ptr->next = (size_t*)new_block;
			}


			ADVANCE_HEAP_POINTER(mem_io.heap,size);
			curr_ptr = new_block;
			break;
		}
		


		// if it aint null , cast it as a block
		mem_io_block *curr_block = curr_ptr; 
		if (IS_MEM_IO_FLAG_USED(curr_block->flags)|| curr_block->size < size){
			prev_ptr = curr_ptr;
			curr_ptr = (mem_io_block*)curr_block->next;
			continue;
		}

		bool should_segment = curr_block->size  > (size + sizeof(mem_io_block));
		if(should_segment) __split_block(curr_block,size);
		
		__init_block(curr_block,curr_block->next,curr_block->prev,size);

		curr_ptr = curr_block; 
		break;
	}

	void* return_pointer = (void*)curr_ptr+sizeof(mem_io_block);
	mem_io.allocated++;
	mem_io.count++;

	pthread_mutex_unlock(&mem_lock);
	return return_pointer;
}











/*
 * unlinke malloc this one's job is to check for solving fragmented memory issues and also
 * merge free unused blocks so we dont get extra memory for no reason 
 * it merge next and prev of the block until they are not free to do so.
 * and also validates the block so it can see if it matches the magic number 
 * otherwise marks the block as unusable and corrupted
*/
void mem_io_free(void* _addr) {
    pthread_mutex_lock(&mem_lock);
    mem_io_block* block = (mem_io_block*)(_addr-sizeof(mem_io_block));
    
    // TODO: maybe a process to cleanup corrupt memeory too ?
    bool is_block_valid = VALIDATE_BLOCK(block);
    if (!is_block_valid){
	block->flags = MEM_IO_FLAG_CORRUPTED; 
	mem_io.errors++;
	mem_io.count--;
	mem_io.deallocated++;
	pthread_mutex_unlock(&mem_lock);
	return;	
     }

    mem_io_block* prev_block = (mem_io_block*) block->prev;
    mem_io_block* next_block = (mem_io_block*) block->next; 

    while(prev_block != NULL && IS_MEM_IO_FLAG_FREE(prev_block->flags)){
		__merge_blocks(prev_block,block);
		block = prev_block; 
		prev_block = (mem_io_block*)prev_block->prev; 
    }


    while(next_block != NULL && IS_MEM_IO_FLAG_FREE(next_block->flags)){
	__merge_blocks(block,next_block);
	next_block = (mem_io_block *)next_block->next;
    }


    block->flags = MEM_IO_FLAG_FREE;

    mem_io.count--;
    mem_io.deallocated++;
    pthread_mutex_unlock(&mem_lock);
    return;
}



/*
* Realloc : reallocate the given memeory , it either shrinks or grows the memory 
* although with some edge cases from glibc standard
* if the addr is null , the we malloc for that given size and return that
* if the size is 0 , then the user is basically asking for deletion so we delete that block
* otherwise we check if the user wants to shrink or grow the memory
* if it is growing we malloc another one and then copy the old data and then return that
* otherwise we just split the segment if it is worthy of being splitted
*/
void* mem_io_realloc(void* _addr,size_t size){
	if (_addr == NULL) 
		return mem_io_malloc(size);
	if (_addr != NULL && size == 0){
		mem_io_free(_addr);
		return NULL;
	}
	
	pthread_mutex_lock(&mem_lock);
	mem_io_block* block = (mem_io_block*)((uint8_t*)_addr-sizeof(mem_io_block));

	bool is_block_valid = VALIDATE_BLOCK(block);
	if(!is_block_valid){
		mem_io.errors++;
		pthread_mutex_unlock(&mem_lock);
		return NULL; 
	}

	if(size > block->size){
		pthread_mutex_unlock(&mem_lock);

		void *allocated_memory = mem_io_malloc(size);
		memcpy(
			allocated_memory,
			(uint8_t* )block+sizeof(mem_io_block),
			block->size
		);
		mem_io_free(_addr);
		return allocated_memory;
	// if the size isnt big enough to store the header for the next block then we dont really
	// need to split , cause it aint of any use
	}else if (size < block->size && (block->size - size) > sizeof(mem_io_block) ){
		__split_block(block,size);
	}
	
	pthread_mutex_unlock(&mem_lock);
	// if now realloc is needed then we are good
	return _addr;
}

void mem_io_stack_trace(){
	mem_io_block* head = mem_io.head;
	while(head != NULL){
		block_debug(head);
		head = (mem_io_block*)head->next;
	}
}


void mem_io_log_trace(){
	printf(
	"\n\n\nDEBUG INFO:\n\nseg_break: %p\nheap: %p\nhead: %p\ncount: %ld\nallocated: %d\ndeallocated: %d\n\n\n",
	mem_io.seg_break,
	mem_io.heap,
	mem_io.head,
	mem_io.count,
	mem_io.allocated,
	mem_io.deallocated
	);
}
