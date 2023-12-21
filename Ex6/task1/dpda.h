#include "stack.h"

struct dpda {
	int initial;
	int (*transition)(int state, char ch, stack_t *s);
};

struct dpda_ctx {
	struct dpda *dpda;
	stack_t *stack;
	int state;
	char* input;
	int offset;
};



int dpda1_transition (int st, char ch, stack_t *s);

struct dpda *create_dpda (int i, int (*t)(int, char, stack_t*));

struct dpda_ctx *dpda_new_ctx (struct dpda *dpda, char *word);

int run_dpda (struct dpda_ctx *ctx);

int delete_dpda_ctx (struct dpda_ctx *ctx);

int delete_dpda(struct dpda* dpda);
