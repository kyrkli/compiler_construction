#include "ast.h"
#include <stdio.h>
#include <stdlib.h>

astnode_t *astnode_new (int type) { 
	astnode_t *a = calloc (sizeof *a, 1); 
	a->type = type; 
	return a; 
}

int execute_ast (astnode_t *root) {
	switch (root->type) { 
		case stmts: 
			execute_ast(root->child[0]); 
			return execute_ast(root->child[1]); 
		case '+': 
			return execute_ast(root->child[0]) + execute_ast(root->child[1]); 
		case num: 
			return root->val.num; 
		case id: 
			return getvar(root->val.str); 
		case '=': 
			return setvar(root->val.str, execute_ast(root->child[1])); 
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
		if (root->child[i]) { 
			// Create graph edge 
			fprintf(dot, " n%d -> n%d\n", root->id, root->child[i]->id); 
			print_ast(root->child[i], depth+1);
		} 
	} 

	if (depth == 0) { 
		fprintf(dot, "}\n"); 
		fclose(dot); 
	} 
}
