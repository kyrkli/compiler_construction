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
	runtime_error(0, "The variable is already declared in the same block\n");	
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
		case _bool:
    		if(*(int *) gval > 0)
				*(int *) gval = 1;
			else *(int *) gval = 0;
			s_push(&globals, (stackval_t) { .type = type, .gval.int_val = *(int *) gval, .id = strdup(id) });
			break;
		case _charptr:
    		s_push(&globals, (stackval_t) { .type = type, .gval.charptr_val = strdup((char *) gval), .id = strdup(id) });
			break;
		default://TODO another types
			assert(0);
			break;
	}
	}

  return gval;
}

void* var_declare (type_t type, char *id, void* gval) {
  stackval_t *s = var_lookup (id, VAR_BORDER_BLOCK);
  if (s) {
    // Handle multiple declaration in same block
	runtime_error(0, "The variable is already declared in the same block\n");	
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
		case _bool:
			if(*(int *) gval > 0)
				*(int *) gval = 1;
			else *(int *) gval = 0;
			s_push(&vars, (stackval_t) { .type = type, .gval.int_val = *(int *) gval, .id = strdup(id) });
			break;
		case _charptr:
    		s_push(&vars, (stackval_t) { .type = type, .gval.charptr_val = strdup((char *) gval), .id = strdup(id) });
			break;
		default://TODO another types
			assert(0);
			break;
	}
  }

  return gval;
}

void* var_set (char *id, void* gval, type_t expected_type) {
  stackval_t *s = var_lookup (id, VAR_BORDER_FUNC);
  if (s){
	  runtime_error(s->type == expected_type,
			  		"Uncorrect type of the assigning value to the variable\n");
	  switch(s->type){
		case _char:
			s->gval.char_val = *(char *) gval;
			break;
		case _int:
			s->gval.int_val = *(int *) gval;
			break;
		case _double:
			s->gval.double_val = *(double *) gval;
			break;
		case _bool:
			if(*(int *) gval > 0)
				*(int *) gval = 1;
			else *(int *) gval = 0;
			s->gval.int_val = *(int *) gval;
			break;
		case _charptr:
			s->gval.charptr_val = strdup((char *) gval);
			break;
		default://TODO another types
			assert(0);		
	}

  } else {
    // Handle usage of undeclared variable
	printf("id of variable = %s\n", id);
	var_dump();
	runtime_error(0, "The used variable is NOT declared in the block\n");	
  }

  return gval;
}

stackval_t var_get (char *id) {
	stackval_t *s = var_lookup(id, VAR_BORDER_FUNC);
	if (s)
		return *s;
	else {
    	// Handle usage of undeclared variable
		runtime_error(0, "Detected usage of undeclared variable\n");
		return (stackval_t) {};
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
				print_gdata(run->data);
		}
		run = run->next;
	}
	printf("-- BOTTOM --\n");
  	
	run = globals.head;
	while(run){
		print_gdata(run->data);	
		run = run->next;
	}
	printf("-- GLOBALS --\n\n");
}

void set_general_svar(stackval_t svar){
	switch(svar.type){
		case _char:
			var_set(svar.id, &( (char) {svar.gval.char_val}), svar.type);	
			break;
		case _int:
		case _bool:
			var_set(svar.id, &( (int) {svar.gval.int_val}), svar.type);	
			break;
		case _double:
			var_set(svar.id, &( (double) {svar.gval.double_val}), svar.type);	
			break;
		case _charptr:
			var_set(svar.id, &( (char*) {svar.gval.charptr_val}), svar.type);	
			break;
	}
}

void set_general_svar_zero(stackval_t* svar){
	switch(svar->type){
		case _char:
			svar->gval.char_val = '\0';
			break;
		case _int:
		case _bool:
			svar->gval.int_val = 0;
			break;
		case _double:
			svar->gval.double_val = 0.0;
			break;
		case _charptr:
			svar->gval.charptr_val = NULL;
			break;
	}

}

void var_declare_general_zero(stackval_t svar, char mod){
	assert(mod == 'g' || mod == 'l');
	switch(svar.type){
		case _char:
			if(mod == 'g')
				var_declare_global(svar.type, svar.id, &( (char) {'\0'} ));
			else var_declare(svar.type, svar.id, &( (char) {'\0'} ));
			break;
		case _int:
			if(mod == 'g')
				var_declare_global(svar.type, svar.id, &( (int) {0} ));
			else var_declare(svar.type, svar.id, &( (int) {0} ));
			break;
		case _double:
			if(mod == 'g')
				var_declare_global(svar.type, svar.id, &( (double) {0.0} ));
			else var_declare(svar.type, svar.id, &( (double) {0.0} ));
			break;
		case _bool:
			if(mod == 'g')
				var_declare_global(svar.type, svar.id, &( (int) {0} ));
			else var_declare(svar.type, svar.id, &( (int) {0} ));
			break;
		case _charptr:
			if(mod == 'g')
				var_declare_global(svar.type, svar.id, NULL);
			else var_declare(svar.type, svar.id, NULL);
			break;
	}

		
}

void var_declare_general_val(stackval_t svar, char mod){
	assert(mod == 'g' || mod == 'l');
	if(mod == 'g')
		var_declare_global(svar.type, svar.id, &svar.gval);
	else {
		if(svar.type == _charptr)
			var_declare(svar.type, svar.id, (void *) svar.gval.charptr_val);
		else
			var_declare(svar.type, svar.id, &svar.gval);

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
