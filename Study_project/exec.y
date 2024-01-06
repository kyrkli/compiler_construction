%{
//includes
#include <stdio.h>
#include <math.h>
#include "variable_handling.h"

int yylex(void);
void yyerror(const char* msg){
	fprintf(stderr, "%s\n", msg);
}

%}
%define parse.error verbose

%code requires {

}


%union {
	int num;
	Types_t type;
	char* str;
}
%start S

//terminals
%token <num> num 
		<str> id
		<type> type
		<str> newline
//non-terminals

%right '='
%left '-' '+'  
%left '*' '/'
%right '^'

%type <str> DECLARE 
%type <num> NEXT TERM
%%
S : S NEXT
  | %empty

NEXT: DECLARE { var_declare_global($1, 0); }
	| DECLARE '=' TERM { var_declare_global($1, $3); } 
	//next functionality

DECLARE: type '-' '>' id newline { $$ = $4;} 

TERM: TERM '-' TERM { $$ = $1 - $3; }
	| TERM '+' TERM { $$ = $1 + $3; }
	| TERM '*' TERM { $$ = $1 * $3; }
	| TERM '/' TERM { $$ = $1 / $3; }
	| TERM '^' TERM { $$ = pow($1, $3); }
	| num { $$ = $1; }


%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
