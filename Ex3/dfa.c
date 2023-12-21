#include "dfa.h"
#include <stdlib.h>
#include <string.h>

int dfa1_transition(int st, char ch){
	switch(st){
	case 1: switch(ch) {
		case '0': return 1;
		case '1': return 2;
		default : return 0;
		}
	
	case 2: switch(ch) {
		case '0': return 3;
		case '1': return 2;
		default : return 0;
		}
	
	case 3: switch(ch) {
		case '0': return 1;
		case '1': return 4;
		default : return 0;
		}
	
	case 4: switch(ch) {
		case '0': return -5;
		case '1': return 4;
		default : return 0;
		}
	
	case -5: switch(ch) {
		case '0': return -6;
		case '1': return 4;
		default : return 0;
		}
	
	case -6: switch(ch) {
		case '0': return -6;
		case '1': return 2;
		default : return 0;
		}
	}
	return 0;

}

struct dfa *create_dfa(int i, int (*t)(int, char)){
	struct dfa *dfa = malloc(sizeof(struct dfa));
	dfa->initial = i;
	dfa->transition = t;
	return dfa;
}

struct dfa_ctx *dfa_new_ctx(struct dfa *dfa, char *word){
	struct dfa_ctx *ctx = malloc (sizeof(struct dfa_ctx));
	ctx->dfa = dfa;
	ctx->state = dfa->initial;
	ctx->input = strdup(word);
	ctx->offset = 0;
	return ctx;
}

int run_dfa(struct dfa_ctx *ctx){
	while(ctx->input[ctx->offset] != '\0'){
		// Input word not fully consumed yet
		// Get next character and advance offset
		char ch = ctx->input[ctx->offset++];
		// Call the transition function of the dfa
		ctx->state = ctx->dfa->transition(ctx->state, ch);
		if(ctx->state == 0)
			return 0; // We hit an invalid state, bail out
	}
	return ctx->state < 0;
}

