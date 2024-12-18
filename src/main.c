#include <stdio.h>
#include <string.h>
#include "memory.h"


uint8_t* allocated_large_example(){
	uint8_t* large = (uint8_t*)mem_io_malloc(1024*130);
	return large;
}


int main(){

/*
	char* b = (char*)mem_io_realloc(NULL,sizeof(char)*100);
	printf("curr_pointer b = %p\n",b);
	strcpy(b,"hello world\n");
	//mem_io_stack_trace();
	printf("b is %s\n",b);


	char* d = (char*)mem_io_malloc(10);
	mem_io_free(d);
	char*c = (char*)mem_io_realloc(b,101);
	printf("curr_pointer c = %p\n",c);
	printf("c is %s\n",c);
	mem_io_stack_trace();
	mem_io_log_trace();
*/

	//mem_io_realloc(c,0);
	printf("sizeof block %ld\n",sizeof(mem_io_block));
//	mem_io_stack_trace();
	uint8_t* z =  allocated_large_example();
	printf("curr_pointer z = %p\n",z);
	mem_io_realloc(z,1024*200);
	mem_io_log_trace();

	/*
	char* b = (char*)mem_io_malloc(sizeof(char)*40);
	printf("curr_pointer b = %p\n",b);
	
	char* d = (char*)mem_io_malloc(sizeof(char)*50);
	printf("curr_pointer d = %p\n",d);
	char* e = (char*)mem_io_malloc(sizeof(char)*3);
	printf("curr_pointer e = %p\n",e);
	char* m = (char*)mem_io_malloc(sizeof(char)*4);
	printf("curr_pointer m = %p\n",m);

	
	printf("before stack trace\n");
	mem_io_stack_trace();
	
	printf("after free b\n");

	mem_io_free(b);
	
	
	mem_io_stack_trace();
	
	printf("after free d\n");

	mem_io_free(d);

	mem_io_stack_trace();
	printf("after free e\n");

	mem_io_free(e);
	printf("after free m\n");

	mem_io_stack_trace();


	mem_io_free(m);

	mem_io_stack_trace();

	mem_io_log_trace();
	*/

	/*

	char *q = (char*)mem_io_malloc(sizeof(char) * 90);
	printf("curr_pointer q = %p\n",q);
	mem_io_stack_trace();

	mem_io_log_trace();
	*/
}
