%{
//includes
#include <stdio.h>

int yylex(void);
void yyerror(const char* msg){
	fprintf(stderr, "%s\n", msg);
}

typedef enum {
	_char = 0,
	_int,
	_float,
	_double
} Types;

%}
%define parse.error verbose
%union {
	int num;
	Types type;
	char* id;
}
%start S

//terminals
%token <num> num 
		<id> id
		<type> type
//non-terminals

%right '='
%left '-' '+'  
%left '*' '/'
%right '^'

%%
S : S NEXT
  | %empty

NEXT: DECLARE
	| DECLARE '=' TERM
	//next functionality

DECLARE: type '-' '>' id '\n' { printf("DECLARE parses\n"); } 

TERM: TERM '-' TERM
	| TERM '+' TERM
	| TERM '*' TERM
	| TERM '/' TERM
	| TERM '^' TERM
	| num { }
	| id { /*variables*/ }


%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
