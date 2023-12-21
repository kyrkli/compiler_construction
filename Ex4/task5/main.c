#include <stdio.h>
#include "stack.h"

int main(){
	stack_t *my_stack = s_new();
	int add_array[8] = {3, 1, 3, 3, 7, 2, 4, 8};
   	
	s_push(my_stack, add_array[0]);	
   	s_push(my_stack, add_array[1]);	
   	s_push(my_stack, add_array[2]);	
   	s_push(my_stack, add_array[3]);	
   	s_push(my_stack, add_array[4]);	
	
	printf("size = %d\n", my_stack->size);
	printf("pop value1 = %d\n", s_pop(my_stack));
	printf("pop value2 = %d\n", s_pop(my_stack));
	printf("pop value3 = %d\n", s_pop(my_stack));
	
	printf("size = %d\n", my_stack->size);
   	s_push(my_stack, add_array[5]);	
   	s_push(my_stack, add_array[6]);	
   	s_push(my_stack, add_array[7]);	
	
	printf("size = %d\n", my_stack->size);
	printf("pop value4 = %d\n", s_pop(my_stack));
	printf("pop value5 = %d\n", s_pop(my_stack));
	printf("pop value6 = %d\n", s_pop(my_stack));
	printf("pop value7 = %d\n", s_pop(my_stack));
	printf("pop value8 = %d\n", s_pop(my_stack));
	printf("size = %d\n", my_stack->size);
	free_stack(my_stack);
	return 0;
}
