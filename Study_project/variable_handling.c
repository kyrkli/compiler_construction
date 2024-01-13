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

void* var_declare_global (type_t type, char *id, void* gval) {
	stackval_t *s = var_lookup (id, 0);
	if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    switch(type){
		case _char:
			s->gval.char_val = *(char *) gval;
			break;
		case _int:
			s->gval.int_val = *(int *) gval;
			break;
		case _double:
			s->gval.double_val = *(double *) gval;
			break;
		default://TODO another types
			break;
	}
	} else {
	//TODO gval = strdup(gval) in case of pointer
	switch(type){
		case _char:
    		s_push(&globals, (stackval_t) { .type = type, .gval.char_val = *(char *) gval, .id = strdup(id) });
			break;
		case _int:
    		s_push(&globals, (stackval_t) { .type = type, .gval.int_val = *(int *) gval, .id = strdup(id) });
			break;
		case _double:
    		s_push(&globals, (stackval_t) { .type = type, .gval.double_val = *(double *) gval, .id = strdup(id) });
			break;
		case _charptr:
    		//s_push(&globals, (stackval_t) { .type = type, .gval.charptr_val = strdup(*(char **) gval), .id = strdup(id) });
			break;
		case _intptr:
    		//s_push(&globals, (stackval_t) { .type = type, .gval.intptr_val = strdup(*(int **) gval), .id = strdup(id) });
			break;
		case _floatptr:
    		//s_push(&globals, (stackval_t) { .type = type, .gval.floatptr_val = strdup(*(float **) gval), .id = strdup(id) });
			break;
		case _doubleptr:
    		//s_push(&globals, (stackval_t) { .type = type, .gval.doubleptr_val = strdup(*(double **) gval), .id = strdup(id) });
			break;
		default://TODO good default
			break;
	}
	}

  return gval;
}

void* var_declare (type_t type, char *id, void* gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_BLOCK);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
     switch(type){
		case _char:
			s->gval.char_val = *(char *) gval;
			break;
		case _int:
			s->gval.int_val = *(int *) gval;
			break;
		case _double:
			s->gval.double_val = *(double *) gval;
			break;
		default://TODO another types
			break;
	}   
  } else {
	//TODO gval = strdup(gval) in case of pointer
  	switch(type){
		case _char:
    		s_push(&vars, (stackval_t) { .type = type, .gval.char_val = *(char *) gval, .id = strdup(id) });
			break;
		case _int:
    		s_push(&vars, (stackval_t) { .type = type, .gval.int_val = *(int *) gval, .id = strdup(id) });
			break;
		case _double:
    		s_push(&vars, (stackval_t) { .type = type, .gval.double_val = *(double *) gval, .id = strdup(id) });
			break;
		case _charptr:
    		//s_push(&vars, (stackval_t) { .type = type, .gval.charptr_val = strdup(*(char **) gval), .id = strdup(id) });
			break;
		case _intptr:
    		//s_push(&vars, (stackval_t) { .type = type, .gval.intptr_val = strdup(*(int **) gval), .id = strdup(id) });
			break;
		case _floatptr:
    		//s_push(&vars, (stackval_t) { .type = type, .gval.floatptr_val = strdup(*(float **) gval), .id = strdup(id) });
			break;
		case _doubleptr:
    		//s_push(&vars, (stackval_t) { .type = type, .gval.doubleptr_val = strdup(*(double **) gval), .id = strdup(id) });
			break;
		default://TODO good default
			break;
	}
  }

  return gval;
}

void* var_set (char *id, void* gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s){
      type_t type = s->type; 
	  switch(type){
		case _char:
			s->gval.char_val = *(char *) gval;
			break;
		case _int:
			s->gval.int_val = *(int *) gval;
			break;
		case _double:
			s->gval.double_val = *(double *) gval;
			break;
		default://TODO another types
			break;
	}

  } else {
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    //TODO JUST IGNORE var_declare(id, gval);
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

void var_declare_global_zero (type_t type, char *id){
	switch(type){
		case _char:
			var_declare_global(type, id, &( (char) {'\0'} ));
			break;
		case _int:
			var_declare_global(type, id, &( (int) {0} ));
			break;
		case _float:
			var_declare_global(type, id, &( (float) {0.0} ));
			break;
		case _double:
			var_declare_global(type, id, &( (double) {0.0} ));
			break;
		case _charptr:
		case _intptr:
		case _floatptr:
		case _doubleptr:
		default://TODO good default
			break;

	}
	
}

#ifdef TEST
int main (void) {
  var_enter_function(); var_dump();
  var_declare_global( _int, "a", &((double){2121.21}));
  var_declare( _int, "a", &((double) {100.10})); var_dump();
  var_declare( _int, "b", &((double) {200.20})); var_dump();
  printf("%f\n", var_get("a").double_val);
  var_enter_function(); var_dump();
  printf("%f\n", var_get("a").double_val);
  var_declare(_int, "a", &((double) {42.42})); var_dump();
  var_declare(_int, "x", &((double) {432.43})); var_dump();
  printf("%f\n", var_get("a").double_val);                                                           
  var_enter_block(); var_dump();
  var_declare(_int, "a", &((double) {9999.99})); var_dump();
  var_set("x", &((double) {10000.10})); var_dump();                                                  
  printf("%f\n", var_get("a").double_val);
  printf("%f\n", var_get("x").double_val);
  var_leave_function(); var_dump();
  var_leave_function(); var_dump();
  printf("%f\n", var_get("a").double_val); 
/*
  var_enter_function(); var_dump();
  var_declare_global( _double, "a", &((double){2121.21}));
  var_declare( _double, "a", &((double) {100.10})); var_dump();
  var_declare( _double, "b", &((double) {200.20})); var_dump();
  printf("%f\n", var_get("a").double_val);
  var_enter_function(); var_dump();
  printf("%f\n", var_get("a").double_val);
  var_declare(_double, "a", &((double) {42.42})); var_dump();
  var_declare(_double, "x", &((double) {432.43})); var_dump();
  printf("%f\n", var_get("a").double_val);                                                           
  var_enter_block(); var_dump();
  var_declare(_double, "a", &((double) {9999.99})); var_dump();
  var_set("x", &((double) {10000.10})); var_dump();                                                  
  printf("%f\n", var_get("a").double_val);
  printf("%f\n", var_get("x").double_val);
  var_leave_function(); var_dump();
  var_leave_function(); var_dump();
  printf("%f\n", var_get("a").double_val);

*/
}
#endif
