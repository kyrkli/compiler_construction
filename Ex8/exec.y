%{
#include <stdio.h>
int yylex(void);
void yyerror(char* msg){
	fprintf(stderr, "%s\n", msg);
}
%}
%union {
	int num;
	char ch;
	char* id;
}
%token num op id
%start S
%%
S: T | ASS T
T: G G op
ASS: id ':' '='
G: IN | T
IN: num | id
%%
int main(void) {
	int st = yyparse();	
	printf("Word is %i\n", st);
	return st; 
}
