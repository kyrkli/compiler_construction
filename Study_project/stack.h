typedef enum {
	_char = 0,
	_int,
	_float,
	_double,
	_charptr,
	_intptr,
	_floatptr,
	_doubleptr
} type_t;  

typedef union {
	char char_val;
	int int_val;
	float float_val;
	double double_val;
	char* charptr_val;
	int* intptr_val;
	float* floatptr_val;
	double* doubleptr_val;
} value_t;

typedef struct {
	type_t type;
	char *id;
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
void print_gdata(node_t* node);
void show_stack(stack_t *s);

stackval_t* s_peek(stack_t *s);
stackval_t* s_lookup(stack_t *s, char *id);
