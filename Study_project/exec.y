%{
#include <stdio.h>
#include <math.h>
#include "variable_handling.h"
#include "ast.h"
#include <assert.h>
#include <time.h>
#include <stdlib.h>

extern int yylineno;
void yyerror(const char *s) {
	printf("Parse error in line %d: %s\n", yylineno, s);
}
int yylex (void);
int yydebug = 0;

%}
%define parse.error verbose

%union {
	type_t type;
	char* str;
	stackval_t svar;

	astnode_t *ast;
}
%start S

//terminals
%token 	<svar> num
		<svar> boolean
		<svar> id
		<type> type
		<svar> ch
		<svar> str
		<str> newline
		<svar> nl
%token d t and _random or xor not _if _else print _for size func call pp _scanf mod _while arrow el

%left or
%left and
%left xor
%right not

%right '='
%left '-' '+'
%left '*' '/'
%left mod
%right '^'

//non-terminals
%type <ast> S PROG GETRANDOM PRINTVALS BLOCK ARRDEF GETARR ARRAY LENGTH ELEMENTS GLVARDEF SETVAR BOOLTERM INC APARAM SCANF WHILE FUNCCALL FPARAM FUNCDEF STARTFOR FOR PRINT COND IF SCOMPARE COMPARE VARIABLE STERM TERM VARDEF LVARDEF GLOBAL LOCAL SLOCAL SLOCAL2
%%
S : PROG { execute_ast($1); print_ast($1, 0); }

PROG : PROG GLOBAL { $$ = astnode_new(PROG);
	 				 $$->child[0] = $1; $$->child[1] = $2; }
	 | PROG BLOCK { $$ = astnode_new(PROG);
	 				$$->child[0] = $1; $$->child[1] = $2; }
	 | %empty { $$ = NULL; }
//global functionality
GLOBAL 	: GLVARDEF { $$ = astnode_new(GLOBAL);
					 $$->child[0] = $1; }
		| SETVAR  { $$ = astnode_new(GLOBAL);
					$$->child[0] = $1; }
	 	| FUNCDEF { $$ = astnode_new(GLOBAL);
	 				$$->child[0] = $1; }

GLVARDEF : VARDEF { $$ = astnode_new(GLVARDEF); $$->child[0] = $1; }
		 | ARRDEF { $$ = astnode_new(GLVARDEF); $$->child[0] = $1; }

BLOCK 	: '%' newline SLOCAL '$' newline { $$ = astnode_new(BLOCK);
	  									 $$->child[0] = $3;}
SLOCAL 	: LOCAL SLOCAL { $$ = astnode_new(SLOCAL);
						 $$->child[0] = $1; $$->child[1] = $2;}
	   	| BLOCK SLOCAL2 { $$ = astnode_new(SLOCAL);
						 $$->child[0] = $1; $$->child[1] = $2; }
		| %empty { $$ = NULL; }
SLOCAL2 : SLOCAL2 LOCAL { $$ = astnode_new(SLOCAL2);
						  $$->child[0] = $1; $$->child[1] = $2;	}
		| SLOCAL2 BLOCK { $$ = astnode_new(SLOCAL2);
						  $$->child[0] = $1; $$->child[1] = $2;	}
		| %empty { $$ = NULL; }

//local functionality
LOCAL	: LVARDEF { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| IF { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| PRINT { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| FOR { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| SETVAR { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| FUNCCALL newline { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| SCANF { $$ = astnode_new(LOCAL); $$->child[0] = $1; }
		| WHILE { $$ = astnode_new(LOCAL); $$->child[0] = $1; }

LVARDEF : VARDEF { $$ = astnode_new(LVARDEF); $$->child[0] = $1; }
		| ARRDEF { $$ = astnode_new(LVARDEF); $$->child[0] = $1; }
//general functionality
VARDEF 	: VARIABLE newline { $$ = astnode_new(VARDEF);
					  		 $$->child[0] = $1; }
		| VARIABLE newline '=' STERM newline  {	$$ = astnode_new(ASSVARDEF);
							 			$$->child[0] = $1; $$->child[1] = $4; }

VARIABLE: type arrow id { $$ = astnode_new(VARIABLE);
						  $$->val.svar = $3; $$->val.svar.type = $1;}

STERM: type arrow TERM {	$$ = astnode_new(STERM);
	 						$$->val.type = $1; $$->child[0] = $3; }

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
	| TERM mod TERM	  { $$ = astnode_new(MOD);
						$$->child[0] = $1; $$->child[1] = $3; }
	| '(' TERM ')' { $$ = $2; }
	| num { $$ = astnode_new(NUM); $$->val.svar = $1; }
	| id { $$ = astnode_new(GETIDVAL); $$->val.svar = $1; }
	| ch { $$ = astnode_new(CHAR); $$->val.svar = $1; }
	| str { $$ = astnode_new(STR); $$->val.svar = $1; }
	| boolean { $$ = astnode_new(BOOLEAN); $$->val.svar = $1; }
	| FUNCCALL { $$ = astnode_new(RETURNCALL); $$->child[0] = $1; }
	| GETARR { $$ = astnode_new(GETARRVAL); $$->child[0] = $1;}
	| GETRANDOM { $$ = astnode_new(RANDOM); $$->child[0] = $1; }
	| nl { $$ = astnode_new(NL); $$->val.svar = $1;}

GETARR: id '[' STERM ']' { $$ = astnode_new(GETARR); $$->val.svar = $1; $$->child[0] = $3;}
	  | id arrow size { $$ = astnode_new(GETARRSIZE); $$->val.svar = $1; }

IF: _if COND newline BLOCK { $$ = astnode_new(IF);
  					 $$->child[0] = $2; $$->child[1] = $4; }
  | _if COND newline BLOCK _else newline BLOCK { $$ = astnode_new(IFELSE);
  					 			 $$->child[0] = $2; $$->child[1] = $4; $$->child[2] = $7; }

COND : BOOLTERM { $$ = astnode_new(COND); $$->child[0] = $1; }

BOOLTERM : BOOLTERM or BOOLTERM	  { $$ = astnode_new(OR); 
		 							$$->child[0] = $1; $$->child[1] = $3;}
		 | BOOLTERM and BOOLTERM  { $$ = astnode_new(AND);
		 							$$->child[0] = $1; $$->child[1] = $3;}
		 | BOOLTERM xor BOOLTERM  { $$ = astnode_new(XOR); 
		 							$$->child[0] = $1; $$->child[1] = $3;}
		 | 			not BOOLTERM { $$ = astnode_new(NOT); $$->child[0] = $2; }
		 | num { $$ = astnode_new(NUM); $$->val.svar = $1; }
		 | boolean { $$ = astnode_new(BOOLEAN); $$->val.svar = $1; }
		 | id { $$ = astnode_new(GETIDVAL); $$->val.svar = $1; }
		 | SCOMPARE { $$ = astnode_new(BOOLTERM); $$->child[0] = $1; }

SCOMPARE : '[' COMPARE ']' { $$ = astnode_new(SCOMPARE); $$->child[0] = $2; }

COMPARE : STERM '>' STERM { $$ = astnode_new(GREATER);
		 					$$->child[0] = $1; $$->child[1] = $3;}
		| STERM '<' STERM { $$ = astnode_new(LESS); 
		 					$$->child[0] = $1; $$->child[1] = $3;}
		| STERM '>' '=' STERM { $$ = astnode_new(GREATEREQ); 
		 						$$->child[0] = $1; $$->child[1] = $4;}
		| STERM '<' '=' STERM { $$ = astnode_new(LESSEQ); 
		 						$$->child[0] = $1; $$->child[1] = $4;}
		| STERM '=' STERM { $$ = astnode_new(EQUAL);
		 					$$->child[0] = $1; $$->child[1] = $3;}
		| STERM '!' STERM { $$ = astnode_new(NOTEQ);
		 					$$->child[0] = $1; $$->child[1] = $3;}

PRINT : print '[' PRINTVALS ']' newline { $$ = astnode_new(PRINT); $$->child[0] = $3; }

PRINTVALS : PRINTVALS ',' STERM { $$ = astnode_new(PRINTVALS); 
		 						 $$->child[0] = $1; $$->child[1] = $3;}
		  | STERM { $$ = astnode_new(PRINTVAL); $$->child[0] = $1; }

WHILE: _while SCOMPARE newline BLOCK { $$ = astnode_new(WHILE);
	 								   $$->child[0] = $2; $$->child[1] = $4; }

FOR: _for '[' STARTFOR SCOMPARE newline INC ']' newline BLOCK { $$ = astnode_new(FOR);
			$$->child[0] = $3; $$->child[1] = $4; $$->child[2] = $6; $$->child[3] = $9;}

STARTFOR: LVARDEF { $$ = astnode_new(STARTFOR); 
				  $$->child[0] = $1; }

INC : '+' arrow num { $$ = astnode_new(INC); 
			  	$$->val.svar = $3; $$->val.svar.id = "+"; }
	| '-' arrow num { $$ = astnode_new(INC); 
			  	$$->val.svar = $3; $$->val.svar.id = "-"; }
	| '*' arrow num { $$ = astnode_new(INC); 
			  	$$->val.svar = $3; $$->val.svar.id = "*"; }
	| '/' arrow num { $$ = astnode_new(INC); 
			  	$$->val.svar = $3; $$->val.svar.id = "/"; }
	| %empty  { $$ = astnode_new(INC);
				$$->val.svar.type = _int; $$->val.svar.gval.int_val = 1; $$->val.svar.id = "+";}
	| d { $$ = astnode_new(INC);
			$$->val.svar.type = _int; $$->val.svar.gval.int_val = 2; $$->val.svar.id = "*";}
	| t { $$ = astnode_new(INC);
			$$->val.svar.type = _int; $$->val.svar.gval.int_val = 3; $$->val.svar.id = "*";}

SETVAR : id '=' STERM newline { $$ = astnode_new(SETVAR); 
	   							$$->val.svar = $1; $$->child[0] = $3; }
	   | id '[' STERM ']' '=' STERM newline   { $$ = astnode_new(SETARR); $$->val.svar = $1; 
												$$->child[0] = $3; $$->child[1] = $6; }

FUNCDEF : func id type '[' newline FPARAM ']' newline BLOCK { $$ = astnode_new(FUNCDEF);
															  $$->val.svar = $2; $$->val.svar.type = $3; 
															  $$->child[0] = $6; $$->child[1] = $9; }
FPARAM  : FPARAM VARIABLE newline { $$ = astnode_new(FPARAM);
	 				 				$$->child[0] = $1; $$->child[1] = $2; }
		| %empty { $$ = NULL; }

FUNCCALL: call id '[' APARAM ']'  { $$ = astnode_new(FUNCCALL);
									$$->child[0] = $4; $$->val.svar = $2;}

APARAM 	: APARAM pp STERM    { $$ = astnode_new(APARAM);
									$$->child[0] = $1; $$->child[1] = $3; }
	 	| %empty { $$ = NULL; }

SCANF : _scanf '[' VARIABLE ']' newline { $$ = astnode_new(SCANF); $$->child[0] = $3; }
	  | _scanf '[' type arrow id '[' STERM ']' ']' newline { $$ = astnode_new(SCANFARR);
	  							$$->val.svar = $5; $$->val.svar.type = $3; $$->child[0] = $7;}
ARRDEF  : ARRAY newline { $$ = astnode_new(ARRDEF); $$->child[0] = $1; }

		| ARRAY newline '=' '[' ELEMENTS ']' newline { $$ = astnode_new(ARRASS);
												$$->child[0] = $1; $$->child[1] = $5; }

ARRAY : VARIABLE arrow LENGTH { $$ = astnode_new(ARRAY);
		 						$$->child[0] = $1; $$->child[1] = $3; }

LENGTH : STERM { $$ = astnode_new(LENGTH); $$->child[0] = $1; }

ELEMENTS : ELEMENTS el STERM  { $$ = astnode_new(ELEMENTS);
		 						$$->child[0] = $1;  $$->child[1] = $3; }
		 | el STERM { $$ = astnode_new(ELEMENT); $$->child[0] = $2; }

GETRANDOM : _random '[' STERM STERM ']' { $$ = astnode_new(GETRANDOM);
										  $$->child[0] = $3; $$->child[1] = $4; }

%%

int main(){
	srand(time(NULL));
	int st = yyparse();
	return st;
}
