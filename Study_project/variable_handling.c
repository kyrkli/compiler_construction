#include "variable_handling.h"

static stack_t vars, globals;

static stackval_t *var_lookup (char *id, int border) {
	node_t *run = vars.head;
	while(run){
		if(run->data->flags >= border)
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

int var_declare_global (char *id, int val) {
  stackval_t *s = var_lookup (id, 0);
  if (s) {
    // Handle multiple declaration in same block
    // Here: Just ignore the new declaration, set new value
    s->val = val;
  } else {
    s_push(&globals, (stackval_t) { .val = val, .id = strdup(id) });
  }

  return val;
}

int var_declare (char *id, int val) {
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
			s_pop(vars);
			break;
		}
		run = run->next;
		s_pop(vars);
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
			s_pop(vars);
			break;
		}
		run = run->next;
		s_pop(vars);
	}
}

void var_dump (void) {
  printf("-- TOP --\n");
  for (int i = vars.size-1; i >= 0; i--) {
    if (vars.vals[i].flags == VAR_BORDER_FUNC) {
      printf("FUNCTION\n");
    } else if (vars.vals[i].flags == VAR_BORDER_BLOCK) {
      printf("BLOCK\n");
    } else {
      printf("%s : %d\n", vars.vals[i].id, vars.vals[i].val);
    }
  }
  printf("-- BOTTOM --\n");
  for (int i = globals.size-1; i >= 0; i--) {
      printf("%s : %d (global)\n", globals.vals[i].id, globals.vals[i].val);
  }
  printf("-- GLOBALS --\n\n");
}

#ifdef TEST
int main (void) {
  var_enter_func(); var_dump();
  var_declare_global("a", 2121);
  var_declare("a", 100); var_dump();
  var_declare("b", 200); var_dump();
  printf("%d\n", var_get("a"));
  var_enter_func(); var_dump();
  printf("%d\n", var_get("a"));
  var_declare("a", 42); var_dump();
  var_declare("x", 432); var_dump();
  printf("%d\n", var_get("a"));
  var_enter_block(); var_dump();
  var_declare("a", 9999); var_dump();
  var_set("x", 10000); var_dump();
  printf("%d\n", var_get("a"));
  printf("%d\n", var_get("x"));
  var_leave_func(); var_dump();
  var_leave_func(); var_dump();
  printf("%d\n", var_get("a"));
}

#endif
