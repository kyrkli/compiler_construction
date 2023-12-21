#include "dpda.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void rules(stack_t *s, int rule_num){
	switch(rule_num){
		case 1:
			s_pop(s);
			s_push(s, 'S');
			s_push(s, 'T');
			break;	
		case 2:
			s_pop(s);
			break;
		case 3:
			s_pop(s);
			s_push(s, 'A');
			s_push(s, '=');
			s_push(s, 'i');
			break;
		case 4:
			s_pop(s);
			s_push(s, 'A');
			break;
		case 5:
			s_pop(s);
			s_push(s, 'V');
			s_push(s, 'M');
			break;
		case 6:
			s_pop(s);
			s_push(s, 'V');
			s_push(s, 'M');
			s_push(s, '+');
			break;
		case 7:
			s_pop(s);
			break;
		case 8:
			s_pop(s);
			s_push(s, 'W');
			s_push(s, 'n');
			break;	
		case 9:
			s_pop(s);
			s_push(s, 'n');
			s_push(s, '*');
			break;
		case 10:
			s_pop(s);
			break;
		default:
	}

}

int dpda1_transition(int st, char ch, stack_t *s){
	//st:	1 - valid
	//		-1 - success
	//		0 - fail
	char top;
	do{
	top = s_peek(s);
	printf("top = %c ch = %c\n", top, ch);
	show_stack(s, 1);	
	//stack is empty
	if(top == NULL){
		if(ch == '\0')
			return -1;
		else return 0;
	}
	
	switch(top){
	case 'i':
	case '=':
	case '+':
	case '*':
	case 'n':
		if(top == ch){
			s_pop(s);
			return 1;
		}
		else return 0;
	default:
	}
	switch(top){
	case 'S': 	switch(ch) {
				case 'i':
				case 'n':
					rules(s, 1);
					break;
				case '\0':
					rules(s, 2);
					break;
				default:
					return 0;
			 	}
				break;
	case 'T':	switch(ch) {
				case 'i':
					rules(s, 3);
					break;
				case 'n':
					rules(s, 4);
					break;
				default:
					return 0;
				}
				break;	
	case 'A':	switch(ch) {
				case 'n':
					rules(s, 5);
					break;
				default:
					return 0;
			 	}
			  	break;	
	case 'M':	switch(ch) {
				case 'n':
					rules(s, 8);
					break;
				default:
					return 0;
				}
				break;
	case 'V':	switch(ch) {
				case 'i':
				case 'n':
				case '\0':
					rules(s, 7);
					break;
				case '+':
					rules(s, 6);
					printf("problem rule \n");
					show_stack(s, 1);
					break;
				default:
					return 0;
				}
				break;
	case 'W':	switch(ch) {
				case 'i':
				case '+':
				case 'n':
				case '\0':
					rules(s, 10);
					break;
				case '*':
					rules(s, 9);
					break;
				default:
					return 0;
				}
				break;
	default: NULL; 
	}
	printf("top at the end = %c\n\n", top);
	}while(top == 'S' || top == 'T' || top == 'A' || top == 'M' || top == 'V' || top == 'W');
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
	s_push(ctx->stack, 'S');
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

