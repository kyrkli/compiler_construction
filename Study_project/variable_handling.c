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

value_t var_declare_global (type_t type, char *id, value_t gval) {
  stackval_t *s = var_lookup (id, 0);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->gval = gval;
  } else {
    s_push(&globals, (stackval_t) { .type = type, .gval = gval, .id = strdup(id) });
  }

  return gval;
}

value_t var_declare (type_t type, char *id, value_t gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_BLOCK);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->val = val;
  } else {
    s_push(&vars, (stackval_t) { .val = val, .id = strdup(id) });
  }

  return val;
}

int var_set (char *id, int val) {
  stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s)
    s->val = val;
  else {
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    var_declare(id, val);
  }

  return val;
}

int var_get (char *id) {
  stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s)
    return s->val;
  else {
    // Handle usage of undeclared variable
    // Here: implicitly declare variable
    var_declare(id, 0);
    return 0;
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
				printf("%s : %d\n", run->data.id, run->data.val);
		}
		run = run->next;
	}
	printf("-- BOTTOM --\n");
  	
	run = globals.head;
	while(run){
		printf("%s : %d (global)\n", run->data.id, run->data.val);
		run = run->next;
	}
	printf("-- GLOBALS --\n\n");
}
#define TEST 1
#ifdef TEST
int main (void) {
  var_enter_function(); var_dump();
  var_declare_global("a", 2121);
  var_declare("a", 100); var_dump();
  var_declare("b", 200); var_dump();
  printf("%d\n", var_get("a"));
  var_enter_function(); var_dump();
  printf("%d\n", var_get("a"));
  var_declare("a", 42); var_dump();
  var_declare("x", 432); var_dump();
  printf("%d\n", var_get("a"));
  var_enter_block(); var_dump();
  var_declare("a", 9999); var_dump();
  var_set("x", 10000); var_dump();
  printf("%d\n", var_get("a"));
  printf("%d\n", var_get("x"));
  var_leave_function(); var_dump();
  var_leave_function(); var_dump();
  printf("%d\n", var_get("a"));
}

#endif
