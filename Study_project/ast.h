#ifndef AST_H
#define AST_H
#include "stack.h"
#include "variable_handling.h"

// Abstract Syntax Tree Node 
#define MAXCHILDREN 5

typedef enum {
	PROG = 0,
	NEXT,
	GVARDEF,
	ASSVAR,
	VARIABLE,
	STERM,
	NUM,
	GETIDVAL,
	PLUS,
	MINUS,
	MULT,
	DIV,
	EXP
	
} asttype_t;

typedef struct astnode { 
	asttype_t type;
	int id;
	union { 
		char *str;
		type_t type;
		stackval_t svar;
	} val; 
	struct astnode *child[MAXCHILDREN];
} astnode_t;

// Allocate new node of given type 
astnode_t *astnode_new (asttype_t type);

stackval_t execute_ast (astnode_t *root);

void print_ast(astnode_t *root, int depth);

char* node2str(astnode_t* node);

stackval_t calc(stackval_t num1, char op, stackval_t num2);

#endif
