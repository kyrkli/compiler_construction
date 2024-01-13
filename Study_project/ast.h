// Abstract Syntax Tree Node 
#define MAXCHILDREN 5

typedef struct astnode { 
	int type;
	union { 
		int num; 
		char *str; 
	} val; 
	struct astnode *child[MAXCHILDREN];
} astnode_t;

// Allocate new node of given type 
astnode_t *astnode_new (int type);

int execute_ast (astnode_t *root);

void print_ast(astnode_t *root, int depth);

char* node2str(astnode_t node);
