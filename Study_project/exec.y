%{
//includes
#include <stdio.h>

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
	char* id;
}
%start S

//terminals
%token <num> num 
		<id> id
		<type> type
		<id> newline
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
	| DECLARE '=' {printf("after declare =\n");} TERM { printf("term = %d\n", $4); }
	//next functionality

DECLARE: type '-' '>' id newline { printf("DECLARE parsed type = %d id = %s\n", $1, $4); } 

TERM: TERM '-' TERM
	| TERM '+' TERM
	| TERM '*' TERM
	| TERM '/' TERM
	| TERM '^' TERM
	| num { printf("num = %d\n", $1); }


%%

int main(){
	int st = yyparse();
	printf("Word is %i\n", st);
	return st;
}
