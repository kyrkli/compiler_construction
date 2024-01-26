#ifndef AST_H
#define AST_H
#include "stack.h"
#include "variable_handling.h"

// Abstract Syntax Tree Node 
#define MAXCHILDREN 5

typedef enum {
	PROG = 0,
	GLVARDEF,
	MOD,
	WHILE,
	SCANF,
	SETVAR,
	RETURNCALL,
	LOCAL,
	SLOCAL,
	SLOCAL2,
	LVARDEF,
	BLOCK,
	FOR,
	STARTFOR,
	INC,
	PRINT,
	IF,
	IFELSE,
	COND,
	VARDEF,
	FUNCCALL,
	FUNCDEF,
	FPARAM,
	APARAM,
	ARGS,
	GLOBAL,
	ASSVARDEF,
	VARIABLE,
	STERM,
	NUM,
	CHAR,
	STR,
	BOOLTERM,
	SCOMPARE,
	GETIDVAL,
	GREATER,
	LESS,
	EQUAL,
	NOTEQ,
	BOOLEAN,
	OR,
	AND,
	XOR,
	NOT,
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
		//func_t func;
	} val; 
	struct astnode *child[MAXCHILDREN];
} astnode_t;

// Allocate new node of given type 
astnode_t *astnode_new (asttype_t type);

stackval_t execute_ast (astnode_t *root);

void print_ast(astnode_t *root, int depth);

char* node2str(astnode_t* node);

stackval_t compare(stackval_t num1, char op, stackval_t num2);

stackval_t calc(stackval_t num1, char op, stackval_t num2);

stackval_t increment(type_t type, char* id, char op, stackval_t base, stackval_t inc);

astnode_t* find_func(char* id);


stackval_t my_scanf(stackval_t data);

#endif
