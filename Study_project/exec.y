%{
//includes
#include <stdio.h>
#include <math.h>


int yylex(void);
void yyerror(const char* msg){
	fprintf(stderr, "%s\n", msg);
}

%}
%define parse.error verbose

%code requires {
typedef enum {
    _char = 0,
    _int,
    _float,
    _double
} Types_t;

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
%type <num> NEXT DECLARE TERM
%%
S : S NEXT
  | %empty

NEXT: DECLARE
	| DECLARE '=' TERM { setvar(); } 
	//next functionality

DECLARE: type '-' '>' id newline {  } 

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
