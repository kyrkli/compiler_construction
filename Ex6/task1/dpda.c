#include "dpda.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int dpda1_transition(int st, char ch, stack_t *s){
	switch(st){
	case 1: switch(ch) {
		case 'a': 
			s_push(s, ch);
			return 1;
		case 'p': return 2;
		default : return 0;
		}
	
	case 2: switch(ch) {
		case 'b':	
			s_push(s, ch);
			return 2;
		case 'q': return 3;
		default : return 0;
		}
	case 3: switch(ch) {
		case 'c':
			if(s_peek(s) == 'a' || s_peek(s) == 'b'){
				s_pop(s);
				return 3;
			}
		case '\0':
			if(s_peek(s) == NULL)
				return -4;	
		default : return 0;

		}
	}
	return 0;

}

struct dpda *create_dpda(int i, int (*t)(int, char, stack_t*)){
	struct dpda *dpda = malloc(sizeof(struct dpda));
	dpda->initial = i;
	dpda->transition = t;
	return dpda;
}

struct dpda_ctx *dpda_new_ctx(struct dpda *dpda, char *word){
	struct dpda_ctx *ctx = malloc (sizeof(struct dpda_ctx));
	ctx->dpda = dpda;
	ctx->stack = s_new();
	ctx->state = dpda->initial;
	ctx->input = strdup(word);
	ctx->offset = 0;
	return ctx;
}

int run_dpda(struct dpda_ctx *ctx){
	do{
		// Input word not fully consumed yet
		// Get next character and advance offset
		char ch = ctx->input[ctx->offset++];
		// Call the transition function of the dpda
		ctx->state = ctx->dpda->transition(ctx->state, ch, ctx->stack);
		if(ctx->state == 0)
			return 0; // We hit an invalid state, bail out
	} while(ctx->input[ctx->offset - 1] != '\0');
	return ctx->state < 0;
}

