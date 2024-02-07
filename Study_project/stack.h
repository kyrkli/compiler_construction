#ifndef STACK_H
#define STACK_H

typedef enum {
	_error = -1,
	_void,
	_char,
	_int,
	_double,
	_bool,
	_charptr,
	_intptr,
	_doubleptr,
	_boolptr
} type_t;  

typedef union {
	char char_val;
	int int_val;
	double double_val;
	char* charptr_val;
	int* intptr_val;
	double* doubleptr_val;
} value_t;

typedef struct {                                                                                         
	type_t type;
	value_t gval;
} term_t;

typedef struct {
	type_t type;
	char *id;
	int size;
	value_t gval;
	int flags;
} stackval_t;

typedef struct node{
	stackval_t data;
	struct node *next;
} node_t;

typedef struct {
	int size;
	node_t *head;
} stack_t;

stack_t* s_new();
void  s_push(stack_t *s, stackval_t el);
stackval_t s_pop (stack_t *s);
int s_isempty(stack_t *s);
void free_stack(stack_t *s);
void print_gdata(stackval_t data);
void show_stack(stack_t *s);

stackval_t* s_peek(stack_t *s);
stackval_t* s_lookup(stack_t *s, char *id);
void runtime_error(int cond, char* msg);

void printArray(stackval_t sarr);

#endif
