#include "ast.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>

//for keywords @$ = @1 etc with yylinenumber

int ast_id = 0;

astnode_t *astnode_new (asttype_t type) { 
	astnode_t *a = calloc (sizeof *a, 1); 
	a->type = type; 
	a->id = ++ast_id;
	return a; 
}

stackval_t execute_ast (astnode_t *root) {
	if(root == NULL)
		return (stackval_t) {};

	printf("\nstart of execute ast, node = %s\n", node2str(root));
	switch (root->type) { 
		case PROG:
			printf("PROG--------\n");
			execute_ast(root->child[0]);
			root->val.svar = execute_ast(root->child[1]);
			return root->val.svar;
		case NEXT:
			printf("NEXT--------\n");
			root->val.svar = execute_ast(root->child[0]);
			return root->val.svar;
		case GVARDEF: 
			printf("GVARDEF--------\n");
			root->val.svar = execute_ast(root->child[0]);
			var_declare_general_zero(root->val.svar, 'g');
			return root->val.svar;
		case ASSVAR:
			printf("ASSVAR--------\n");
			execute_ast(root->child[0]);
			execute_ast(root->child[1]);
			
			runtime_error(root->child[0]->val.svar.type == root->child[1]->val.svar.type,
							"The type of the variable need to match the type of the assigning value\n");
			//assgning values to the root
			root->val.svar = root->child[0]->val.svar;
			root->val.svar.gval = root->child[1]->val.svar.gval;
			
			var_declare_general_val(root->val.svar, 'g');
			return root->val.svar;
		case VARIABLE:
			printf("VARIABLE--------\n");
			return root->val.svar; 
		case GETIDVAL:
			printf("GETIDVAL--------\n");
			root->val.svar = var_get(root->val.str);	
			return root->val.svar;
		case STERM: 
			root->val.svar = execute_ast(root->child[0]);
			printf("STERM--------\n");
			runtime_error(root->val.type == root->child[0]->val.svar.type,
						 "The type of a term at the moment of declaration need to match the calculated type of the term\n");
			return root->val.svar;
		case NUM:
			printf("NUM--------\n");
			return root->val.svar;
		case PLUS:
			printf("PLUS--------\n");
			root->val.svar = calc(execute_ast(root->child[0]), '+', execute_ast(root->child[1])); 
			return root->val.svar; 
		case MINUS:
			printf("MINUS--------\n");
			root->val.svar = calc(execute_ast(root->child[0]), '-', execute_ast(root->child[1])); 
			return root->val.svar;
		case MULT:
			printf("MULT--------\n");
			root->val.svar = calc(execute_ast(root->child[0]), '*', execute_ast(root->child[1])); 
			return root->val.svar;
		case DIV:
			printf("DIV--------\n");
			root->val.svar = calc(execute_ast(root->child[0]), '/', execute_ast(root->child[1])); 
			return root->val.svar;
		case EXP:
			printf("EXP--------\n");
			root->val.svar = calc(execute_ast(root->child[0]), '^', execute_ast(root->child[1])); 
			return root->val.svar;
		default: 
			assert(1);
	}
}


char* node2str(astnode_t* node){
	switch(node->type){
		case PROG:
			return "PROG";
		case NEXT:
			return "NEXT";
		case GVARDEF:
			return "GVARDEF";
		case ASSVAR:
			return "ASSVAR";
		case VARIABLE:
			return "VARIABLE";
		case STERM:
			return "STERM";
		case NUM:
			return "NUM";
		case GETIDVAL:
			return "GETIDVAL";
		case PLUS:
			return "PLUS";
		case MINUS:
			return "MINUS";
		case MULT:
			return "MULT";
		case DIV:
			return "DIV";
		case EXP:
			return "EXP";
	}
}

void print_ast(astnode_t *root, int depth) { 
	static FILE *dot; 
	if (depth == 0) { 
		dot = fopen("ast.gv", "w"); 
		fprintf(dot, "digraph ast {\n"); 
	} // Create graph node 
	fprintf(dot, " n%d [label=\"%s\"]\n", root->id, node2str(root)); 
	for (int i = 0; i < MAXCHILDREN; i++) {
		if(root->child[i] != NULL) {
			if (root->child[i]) { 
				// Create graph edge
				fprintf(dot, " n%d -> n%d\n", root->id, root->child[i]->id);
				print_ast(root->child[i], depth+1);
			} 
		}
	} 

	if (depth == 0) { 
		fprintf(dot, "}\n"); 
		fclose(dot); 
	} 
}

stackval_t calc(stackval_t num1, char op, stackval_t num2){
	assert(num1.type == num2.type);
	if(num1.type == _int){
		switch(op){
		case '+':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val + num2.gval.int_val};
		case '-':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val - num2.gval.int_val};
		case '*':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val * num2.gval.int_val};
		case '/':
			return (stackval_t) { .type = _int, .gval.int_val = num1.gval.int_val / num2.gval.int_val};
		case '^':
			return (stackval_t) { .type = _int, .gval.int_val = (int) pow(num1.gval.int_val, num2.gval.int_val)};
		default :
			assert(1);
		}
	} else if(num1.type == _double){
		switch(op){
		case '+':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val + num2.gval.double_val};
		case '-':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val - num2.gval.double_val};
		case '*':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val * num2.gval.double_val};
		case '/':
			return (stackval_t) { .type = _double, .gval.double_val = num1.gval.double_val / num2.gval.double_val};
		case '^':
			return (stackval_t) { .type = _double, .gval.double_val = pow(num1.gval.double_val, num2.gval.double_val)};
		default :
			assert(1);
		}
	}
	return (stackval_t) {};
}
