%{
#include <stdio.h>
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
%token 	<num> num 
	<ch> add min mult _div 
	<id> id
%start S
%type <ch> OP
%type <num> S T G
%%
S: T ';' {printf("Result: %d\n", $1);} S | 
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
		}

	} 
G: num {$$ = $1;}| T {$$ = $1;}
OP: add {$$ = $1;} | min {$$ = $1;} | mult {$$ = $1;} | _div {$$ = $1;}

%%
int main(void) {
	int st = yyparse();	
	printf("Word is %i\n", st);
	return st; 
}
