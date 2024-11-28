#include <stdio.h>
#include <string.h>
#include "memory.h"

int main(){
	
	//mem_io_log_trace();
	char* b = (char*)mem_io_malloc(sizeof(char)*40);
	//mem_io_log_trace();
	//strcpy(b,"hello world\n");
	printf("curr_pointer b = %p\n",b);
	//printf("string b is -> %s",b);
	
	char* d = (char*)mem_io_malloc(sizeof(char)*1);
	//strcpy(d,"not hello world\n");
	printf("curr_pointer d = %p\n",d);
	//mem_io_log_trace();
	//printf("string d is -> %s",d);


	//printf("string b is -> %s",b);
	//printf("string d is -> %s",d);
	//mem_io_log_trace();
	
	mem_io_free(b);
	mem_io_free(d);
	//mem_io_log_trace();

	char *q = (char *)mem_io_malloc(sizeof(char)*1);
	char *j = (char *)mem_io_malloc(sizeof(char)*1);

	//strcpy(q,"not not hello world\n");
	printf("curr_pointer q = %p\n",q);	
	printf("curr_pointer j = %p\n",j);	

	//printf("string q is -> %s",q);

	//mem_io_log_trace();
	mem_io_free(j);
	mem_io_free(q);
	char *k = (char*)mem_io_malloc(sizeof(char)*20);
	printf("curr_pointer k = %p\n",k);	

	mem_io_log_trace();

}
