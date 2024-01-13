%{
#include "stack.h"
#include "exec.tab.h"
%}
%option stack noyywrap noinput nounput yylineno
%x STR COML COMB
COMLOPEN [@][@]
COMLCLOSE '\n'

COMBOPEN [@]
COMBCLOSE [#]

STROPEN str[(]
STRCLOSE [)]end

DIGIT09 [0-9]
DIGIT19 [1-9]
DIGIT07 [0-7] 
DIGIT17 [1-7]
DIGIT0F [0-9a-fA-F]
DIGIT1F [1-9a-fA-F]

OCTAL {DIGIT17}{DIGIT07}*|0
HEX {DIGIT1F}{DIGIT0F}*|0
DEC {DIGIT19}{DIGIT09}*|0
ID [a-zA-Z_]+{DIGIT09}*

OCTALDEF octopus->{OCTAL}
HEXDEF hex->{HEX}
DECDEF {DEC}

HEXREAL hex->{HEX}.{HEX}
DECREAL {DEC}.{DEC}

%%
{COMLOPEN} { yy_push_state(YYSTATE); BEGIN(COML);}
{COMBOPEN} { yy_push_state(YYSTATE); BEGIN(COMB);}
{STROPEN} { yy_push_state(YYSTATE); BEGIN(STR);}

{OCTALDEF} { yylval.term.gval.int_val = strtoul(yytext+9, NULL, 8); 
			yylval.term.type = _int;
			return term;}
{DECDEF} { 	yylval.term.gval.int_val = strtoul(yytext, NULL, 10); 
			yylval.term.type = _int;
			return term; }
{HEXDEF} { 	yylval.term.gval.int_val = strtoul(yytext+5, NULL, 16);
			yylval.term.type = _int; 
			return term; }

{HEXREAL} { yytext[3] = '0'; yytext[4] = 'x'; 
			yylval.term.gval.double_val = strtod(yytext+3, NULL); 
			yylval.term.type = _double; 
			return term;}
{DECREAL} { yylval.term.gval.double_val = strtod(yytext, NULL); 
			yylval.term.type = _double; 
			return term;}
				


char { yylval.type = _char; return type; }
int { yylval.type = _int; return type; }
float { yylval.type = _float; return type; }
double { yylval.type = _double; return type; }

{ID} { yylval.str = strdup(yytext); return id; }
\n { yylval.str = strdup(yytext); printf("newline\n"); return newline;}

(\t|\ )

<COML>{COMLCLOSE} { yy_pop_state();}
<COMB>{COMBCLOSE} { yy_pop_state();}
<STR>{STRCLOSE} { yy_pop_state();}

<*>. { printf("default = '%c'\n", *yytext); return *yytext; }
%%
