#include "stack.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>

stack_t* s_new(){
	stack_t *new_stack = (stack_t*) malloc(sizeof(stack_t));
	new_stack->size = 0;
	new_stack->head = NULL;
	return new_stack;
}

void s_push(stack_t *s, stackval_t el){
	node_t *new_head = (node_t*) malloc(sizeof(node_t));
	new_head->data = el;
	new_head->next = s->head;
	s->head = new_head;
	s->size++;
}

stackval_t s_pop(stack_t *s){
	assert(s->size > 0);
	stackval_t head_data = s->head->data;
	node_t *free_node = s->head;
	s->head = s->head->next;
	s->size--;
	free(free_node);		
	return head_data;
}

int s_isempty(stack_t *s){
	return s->size == 0;
}

void free_stack(stack_t *s){
	node_t *free_node = s->head;
	while(free_node != NULL){
		node_t *next_node = free_node->next;
		free(free_node);
		free_node = next_node;
	}
	free(s);
}

void runtime_error(int cond, char* msg){
	if(!cond) {
		printf("!RUNTIME ERROR!\n%s", msg);
		exit(1);
	}
}

void print_gdata(stackval_t data){
	switch(data.type){
    	case _char:
        	printf("(type = %d, size = %d, id = %s, val = %c)\n", data.type, data.size, data.id, data.gval.char_val);
            break;
        case _int:
          	printf("(type = %d, size = %d, id = %s, val = %d)\n", data.type, data.size, data.id, data.gval.int_val);
    	    break;
  	    case _double:
            printf("(type = %d, size = %d, id = %s, val = %f)\n", data.type, data.size, data.id, data.gval.double_val);
            break;
		case _bool:
          	printf("(type = %d, size = %d, id = %s, val = %d)\n", data.type, data.size, data.id, data.gval.int_val);
    	    break;
		case _charptr:
           	printf("(type = %d, size = %d, id = %s, ptr = %p -> val = %s)\n", data.type, data.size, data.id, (void *) data.gval.charptr_val, data.gval.charptr_val);
            break;
		case _intptr:
		case _boolptr:
           	printf("(type = %d, size = %d, id = %s, ptr = %p -> first element = %d):\n", data.type, data.size, data.id, (void *) data.gval.intptr_val, data.gval.intptr_val[0]);
            printf("\t"); printArray(data);
			break;
		case _doubleptr:
			printf("(type = %d, size = %d, id = %s, ptr = %p -> first element = %f):\n", data.type, data.size, data.id, (void *) data.gval.doubleptr_val, data.gval.doubleptr_val[0]);
            printf("\t"); printArray(data);
			break;
		default: 
			printf("type = %d\n", data.type);
			assert(0);
   }
}

void printArray(stackval_t sarr) {
	runtime_error(sarr.size > 0,
				  "Size of array is < 1\n");

	for(int i = 0; i < sarr.size; i++){
		switch(sarr.type){
			case _intptr:
				int* intArr = sarr.gval.intptr_val;
				printf("[%d]:%d -> ", i, intArr[i]);
				break;
			case _boolptr:
				int* boolArr = sarr.gval.intptr_val;
				printf("[%d]:%d -> ", i, boolArr[i]);
				break;
			case _doubleptr:
				double* doubleArr = sarr.gval.doubleptr_val;
				printf("[%d]:%f -> ", i, doubleArr[i]);
				break;
			default:
				assert(0);
		}
	}
	printf("NULL");
}

void show_stack(stack_t *s) {
	node_t *run = s->head;
	
	while(run != NULL){
   		print_gdata(run->data);
		run = run->next;	
	}
}

stackval_t* s_lookup(stack_t *s, char *id){
	node_t *run = s->head;	
	while(run != NULL){
		if(!strcmp(run->data.id, id))
			return &run->data;
		run = run->next;
	}
	return NULL;
}

stackval_t* s_peek(stack_t *s){	
	if(s->size != 0)
		return &s->head->data;
	else
		return NULL;
}
