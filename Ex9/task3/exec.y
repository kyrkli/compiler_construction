%{
//includes
#include <stdio.h>

int yylex(void);
void yyerror(const char* msg){
	fprintf(stderr, "%s\n", msg);
}

%}
%define parse.error verbose
%union { 
	int num;
}

//terminals 
%token 	_true _false _and _or _xor _not
		<num> one zero
%start S

//non-terminals
%type <num> GROUP TERM S

%left _or
%left _and 
%left _xor
%right _not

%%
S : S TERM ';' { printf("Result: %d\n", $2); $$ = $2; }
  | %empty

TERM : GROUP _and GROUP { $$ = $1 && $3; }
	 | GROUP _or GROUP { $$ = $1 || $3; }
	 | GROUP _xor GROUP { $$ = $1 != $3; }
	 | _not GROUP { $$ = !$2;}

GROUP : _true { $$ = 1; }
	  | _false { $$ = 0; }
	  | TERM { $$ = $1; }

%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
//c code
