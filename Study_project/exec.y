%{
#include <stdio.h>
#include <math.h>
#include "variable_handling.h"
#include "ast.h"
#include <assert.h>
//static vars and globals
//syntax errors via assert
//

extern int yylineno;
void yyerror(const char *s) {
	printf("Parse error in line %d: %s\n", yylineno, s);
}
int yylex (void);
int yydebug = 0;

%}
%define parse.error verbose

%code requires {
	//#include "variable_handling.h"
	//#include "stack.h"
}

%union {
	type_t type;
	char* str;
	stackval_t svar;

	astnode_t *ast;
}
%start S

//terminals
%token 	<svar> num 
		<str> id
		<type> type
		<str> newline
//non-terminals

%right '='
%left '-' '+'  
%left '*' '/'
%right '^'

%type <ast> S PROG FUNCDEF NEXT GVARDEF VARIABLE STERM TERM
%%
S : PROG { execute_ast($1); var_dump(); print_ast($1, 0); }

PROG : PROG NEXT  { $$ = astnode_new(PROG);
	 				$$->child[0] = $1; $$->child[1] = $2; }
	 | %empty

//possible functionality
NEXT : GVARDEF { $$ = astnode_new(NEXT);
				$$->child[0] = $1; }
	 | FUNCDEF { $$ = astnode_new(NEXT);
	 			$$->child[0] = $1; }

FUNCDEF : 'f' id type '[' ARGS ']' newline BLOCK 
GVARDEF : VARIABLE	{ $$ = astnode_new(GVARDEF);
					  $$->child[0] = $1; }

	    | VARIABLE '=' STERM newline  {	$$ = astnode_new(ASSVAR);
							 			$$->child[0] = $1; $$->child[1] = $3; }

VARIABLE: type '-' '>' id newline { $$ = astnode_new(VARIABLE);
									$$->val.svar.type = $1; $$->val.svar.id = $4; }

STERM: type '-' '>' TERM {	$$ = astnode_new(STERM);
	 						$$->val.type = $1; $$->child[0] = $4; }

TERM: TERM '-' TERM   { $$ = astnode_new(MINUS);
						$$->child[0] = $1; $$->child[1] = $3; }
	| TERM '+' TERM   { $$ = astnode_new(PLUS);
						$$->child[0] = $1; $$->child[1] = $3; }
	| TERM '*' TERM   { $$ = astnode_new(MULT);
						$$->child[0] = $1; $$->child[1] = $3; }
	| TERM '/' TERM   { $$ = astnode_new(DIV);
						$$->child[0] = $1; $$->child[1] = $3; }
	| TERM '^' TERM   { $$ = astnode_new(EXP);
						$$->child[0] = $1; $$->child[1] = $3; }
	| num { $$ = astnode_new(NUM); $$->val.svar = $1; }
	| id { $$ = astnode_new(GETIDVAL); $$->val.str = $1;}

%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
