%{
#include <stdio.h>
#include "stack.h"
#include <string.h>
stack_t *vars;

int yylex(void);
void yyerror(const char* msg){
	fprintf(stderr, "%s\n", msg);
}
%}
%define parse.error verbose
%union {
	int num;
	char ch;
	char* id;
}

//terminals
%token 	<num> num 
		<ch> add min mult _div 
		<id> id
		ass
%start S

//non terminals
%type <ch> OP
%type <num> S T G
%%

S : S T ';' { printf("Result: %d\n", $1); $$ = $2; } 
  | S id ass T ';' { $$ = $4;
						
						printf("$2 = %s\n", $2);
						val_t *var = s_lookup(vars, $2);
						if(var){
							printf("here inside\n");
							var->nr = $4;
						}
						else {
							printf("here 1\n");
							val_t new_var;
							new_var.id = strdup($2);
							new_var.nr = $4;
							s_push(vars, new_var);

							printf("here 2\n");
						}
						printf("Result: %d\n", $4); 
					}


  | %empty {$$ = 0;}

T: G G OP {	switch($3){
		case '+':
		  $$ = $1 + $2;
		  break;
		case '-':
		  $$ = $1 - $2;
		  break;
		case '*':
		  $$ = $1 * $2;
		  break;
		case '/':
		  $$ = $1 / $2;
		  break;
		default:
		  $$ = 0;
		}

	} 
G: num {$$ = $1;} 
 | T {$$ = $1;}
 | id { val_t* var = s_lookup(vars, $1); 
 		if(var) 
			$$ = var->nr;
		else $$ = 0;
		printf("$$ = %d in G\n", $$);
	}
OP: add {$$ = $1;} 
  | min {$$ = $1;} 
  | mult {$$ = $1;} 
  | _div {$$ = $1;}

%%
int main(void) {
	vars = s_new();
	int st = yyparse();	
	printf("Word is %i\n", st);
	free_stack(vars);
	return st; 
}
