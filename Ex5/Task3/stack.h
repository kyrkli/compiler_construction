typedef int val_t;

typedef struct node{
	val_t data;
	struct node *next;
} node_t;

typedef struct stack{
	int size;
	node_t *head;
} stack_t;

stack_t* s_new();
void  s_push(stack_t *s, val_t num);
val_t s_pop (stack_t *s);
int s_isempty(stack_t *s);
void free_stack(stack_t *s);
