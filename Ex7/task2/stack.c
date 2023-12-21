#include "stack.h"
#include <stdlib.h>
#include <string.h>

stack_t* s_new(){
	stack_t *new_stack = (stack_t*) malloc(sizeof(stack_t));
	new_stack->size = 0;
	new_stack->head = NULL;
	return new_stack;
}

void s_push(stack_t *s, val_t num){
	node_t *new_node = (node_t*) malloc(sizeof(node_t));
	new_node->data = num;
	new_node->next = s->head;
	s->head = new_node;
	s->size++;
}

val_t s_pop(stack_t *s){
	if(s->size != 0){
		val_t head_data = s->head->data;
		node_t *free_node = s->head;
		s->head = s->head->next;
		s->size--;
		free(free_node);		
		return head_data;
	} else
		return NULL;
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

void show_stack(stack_t *s, int code_t) {
	node_t *run = s->head;
	
	switch(code_t){
	case 0:
		while(run != NULL){
			printf("%d\n", run->data);
	   		run = run->next;	
		}
		break;
	case 1:
		while(run != NULL){
			printf("%c\n", run->data);
	   		run = run->next;	
		}
		break;
	default:
		break;
	}
}

val_t s_peek(stack_t *s){	
	if(s->size != 0)
		return s->head->data;
	else
		return NULL;
}
