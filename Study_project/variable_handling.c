#include "variable_handling.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

stack_t vars, globals;

static stackval_t *var_lookup (char *id, int border) {
	node_t *run = vars.head;
	while(run){
		if(run->data.flags >= border)
			break;
		if(!strcmp(run->data.id, id))
			return &run->data;
		run = run->next;
	}
	
	if(border == VAR_BORDER_BLOCK)
		return NULL;

	run = globals.head;
	while(run){
		if(!strcmp(run->data.id, id))	
			return &run->data;
		run = run->next;
	}

	return NULL;
}

value_t var_declare_global (type_t type, char *id, void* gval) {
  stackval_t *s = var_lookup (id, 0);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->gval = gval;
  } else {
	//TODO gval = strdup(gval) in case of pointer
    s_push(&globals, (stackval_t) { .type = type, .gval = gval, .id = strdup(id) });
  }

  return gval;
}

value_t var_declare (type_t type, char *id, void* gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_BLOCK);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    
	s->gval = gval;
  } else {
	//TODO gval = strdup(gval) in case of pointer
    s_push(&vars, (stackval_t) { .type = type, .gval = gval, .id = strdup(id) });
  }

  return gval;
}

value_t var_set (char *id, void* gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s)
    s->gval = gval;
  else {
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    //JUST IGNORE var_declare(id, gval);
  }

  return gval;
}

value_t var_get (char *id) {
	stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
	if (s)
		return s->gval;
	else {
    	// Handle usage of undeclared variable
    	// Here: implicitly declare variable
    	//JUST IGNORE var_declare(id, 0);
		//TODO is it good solution?	
		return (value_t) 0;
	}
}

void var_enter_block (void) {
  s_push(&vars, (stackval_t) { .flags = VAR_BORDER_BLOCK, .id = "" });
}

void var_leave_block (void) {
	node_t *run = vars.head;
	int block_is_found = 0;
  	while(run) {
		if(run->data.flags == VAR_BORDER_BLOCK){
			block_is_found = 1;
			break;
		}
		run = run->next;
  	}
	
	assert(block_is_found == 1);
		
	run = vars.head;
  	while(run) {
		if(run->data.flags == VAR_BORDER_BLOCK){
			s_pop(&vars);
			break;
		}
		run = run->next;
		s_pop(&vars);
	}
}

void var_enter_function (void) {
  s_push(&vars, (stackval_t) { .flags = VAR_BORDER_FUNC, .id = "" });
}

void var_leave_function (void) {
	node_t *run = vars.head;
	int block_is_found = 0;
  	while(run) {
		if(run->data.flags == VAR_BORDER_FUNC){
			block_is_found = 1;
			break;
		}
		run = run->next;
  	}
	
	assert(block_is_found == 1);
	
	run = vars.head;
  	while(run) {
		if(run->data.flags == VAR_BORDER_FUNC){
			s_pop(&vars);
			break;
		}
		run = run->next;
		s_pop(&vars);
	}
}

void var_dump (void) {
	node_t* run = vars.head;
	printf("-- TOP --\n");
	while(run){
		switch(run->data.flags){
			case VAR_BORDER_FUNC:
				printf("FUNCTION\n");
				break;
			case VAR_BORDER_BLOCK:
				printf("BLOCK\n");
				break;
			default:
				print_gdata(run);
		}
		run = run->next;
	}
	printf("-- BOTTOM --\n");
  	
	run = globals.head;
	while(run){
		print_gdata(run);	
		run = run->next;
	}
	printf("-- GLOBALS --\n\n");
}
#define TEST 1
#ifdef TEST
int main (void) {
  var_enter_function(); var_dump();
  var_declare_global( _float, "a", &((float) 2121.21));
  var_declare( _float, "a", &((float) 100.10); var_dump();
  var_declare( _float, "b", &((float) 200.20); var_dump();
  printf("%f\n", var_get("a").float_val);
  var_enter_function(); var_dump();
  printf("%f\n", var_get("a").float_val);
  var_declare(_float, "a", &((float) 42.42); var_dump();
  var_declare(_float, "x", &((float) 432.43); var_dump();
  printf("%f\n", var_get("a").float_val);
  var_enter_block(); var_dump();
  var_declare(_float, "a", &((float)  9999.99); var_dump();
  var_set("x", &((float) 10000.10); var_dump();
  printf("%f\n", var_get("a").float_val);
  printf("%f\n", var_get("x").float_val);
  var_leave_function(); var_dump();
  var_leave_function(); var_dump();
  printf("%f\n", var_get("a").float_val);
/*
  var_enter_function(); var_dump();
  var_declare_global( _int, "a", (value_t) 2121);
  var_declare( _int, "a", (value_t) 100); var_dump();
  var_declare( _int, "b", (value_t) 200); var_dump();
  printf("%d\n", var_get("a").int_val);
  var_enter_function(); var_dump();
  printf("%d\n", var_get("a").int_val);
  var_declare(_int, "a", (value_t) 42); var_dump();
  var_declare(_int, "x", (value_t) 432); var_dump();
  printf("%d\n", var_get("a").int_val);
  var_enter_block(); var_dump();
  var_declare(_int, "a", (value_t) 9999); var_dump();
  var_set("x", (value_t) 10000); var_dump();
  printf("%d\n", var_get("a").int_val);
  printf("%d\n", var_get("x").int_val);
  var_leave_function(); var_dump();
  var_leave_function(); var_dump();
  printf("%d\n", var_get("a").int_val);
*/
}

#endif
