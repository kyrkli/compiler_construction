%{
#include <stdio.h>
#include <math.h>
#include "variable_handling.h"
#include <assert.h>
//static vars and globals
//syntax errors via assert
//

term_t calc(term_t num1, char op, term_t num2){
	assert(num1.type == num2.type);
	if(num1.type == _int){
		switch(op){
		case '+':
			return (term_t) { .type = _int, .gval.int_val = num1.gval.int_val + num2.gval.int_val};
		case '-':
			return (term_t) { .type = _int, .gval.int_val = num1.gval.int_val - num2.gval.int_val};
		case '*':
			return (term_t) { .type = _int, .gval.int_val = num1.gval.int_val * num2.gval.int_val};
		case '/':
			return (term_t) { .type = _int, .gval.int_val = num1.gval.int_val / num2.gval.int_val};
		case '^':
			return (term_t) { .type = _int, .gval.int_val = (int) pow(num1.gval.int_val, num2.gval.int_val)};
		default :
			assert(1);
		}
	} else if(num1.type == _double){
		switch(op){
		case '+':
			return (term_t) { .type = _double, .gval.double_val = num1.gval.double_val + num2.gval.double_val};
		case '-':
			return (term_t) { .type = _double, .gval.double_val = num1.gval.double_val - num2.gval.double_val};
		case '*':
			return (term_t) { .type = _double, .gval.double_val = num1.gval.double_val * num2.gval.double_val};
		case '/':
			return (term_t) { .type = _double, .gval.double_val = num1.gval.double_val / num2.gval.double_val};
		case '^':
			return (term_t) { .type = _double, .gval.double_val = pow(num1.gval.double_val, num2.gval.double_val)};
		default :
			assert(1);
		}
	}
	return (term_t) {};
}

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
	term_t term;
	stackval_t var;
}
%start S

//terminals
%token 	<num> num 
		<str> id
		<type> type
		<str> newline
		<term> term
//non-terminals

%right '='
%left '-' '+'  
%left '*' '/'
%right '^'

%type <var> VARIABLE 
%type <term> NEXT TERM STERM
%%
S : S NEXT
  | %empty
NEXT: VARIABLE newline { var_declare_global_zero($1.type, $1.id); var_dump(); }
	| VARIABLE '=' STERM newline { var_set($1.id, &$3.gval); var_dump(); } 
	//next functionality

VARIABLE: type '-' '>' id newline { $$ = (stackval_t) {.type = $1, .id = $4}; var_declare_global_zero($1, $4); } 

STERM: type '-' '>' TERM { assert($1 == $4.type); $$ = $4;}
//TODO if and exit
TERM: TERM '-' TERM { $$ = calc($1, '-', $3); }
	| TERM '+' TERM { $$ = calc($1, '+', $3); }
	| TERM '*' TERM { $$ = calc($1, '*', $3); }
	| TERM '/' TERM { $$ = calc($1, '/', $3); }
	| TERM '^' TERM { $$ = calc($1, '^', $3); }
	| term { $$ = $1;}


%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
