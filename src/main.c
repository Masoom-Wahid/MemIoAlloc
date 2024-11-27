#include <stdio.h>
#include "memory.h"

int main(){
	int a = 1; 
	char* b = (char*)mem_io_malloc(sizeof(char)*20);
	printf("%d\n",a);
	b = "hello world\n"; 
	printf("curr_pointer b = %p\n",b);
	char* d = (char*)mem_io_malloc(sizeof(char)*200);
	printf("curr_pointer d = %p\n",d);
	d = "not hello world\n";
	printf("string b is -> %s",b);
	printf("string d is -> %s",d);
	
	mem_io_free(&b);
	mem_io_free(&d);
	

}
