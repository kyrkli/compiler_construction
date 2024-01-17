%{
#include "stack.h"
#include "ast.h"
#include "exec.tab.h"
%}
%option stack noyywrap noinput nounput yylineno
%x STR COML COMB
COMLOPEN @@
COMLCLOSE \n

COMBOPEN @
COMBCLOSE #\n

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
{COMLOPEN} { printf("line comment is open\n"); yy_push_state(YYSTATE); BEGIN(COML);}
{COMBOPEN} { printf("block comment is open\n"); yy_push_state(YYSTATE); BEGIN(COMB);}
{STROPEN} { yy_push_state(YYSTATE); BEGIN(STR);}

{OCTALDEF} { yylval.svar.gval.int_val = strtoul(yytext+9, NULL, 8); 
			yylval.svar.type = _int;
			return num;}
{DECDEF} { 	yylval.svar.gval.int_val = strtoul(yytext, NULL, 10); 
			yylval.svar.type = _int;
			return num; }
{HEXDEF} { 	yylval.svar.gval.int_val = strtoul(yytext+5, NULL, 16);
			yylval.svar.type = _int; 
			return num; }

{HEXREAL} { yytext[3] = '0'; yytext[4] = 'x'; 
			yylval.svar.gval.double_val = strtod(yytext+3, NULL); 
			yylval.svar.type = _double; 
			return num;}
{DECREAL} { yylval.svar.gval.double_val = strtod(yytext, NULL); 
			yylval.svar.type = _double; 
			return num;}
				


char { yylval.type = _char; return type; }
int { yylval.type = _int; return type; }
real { yylval.type = _double; return type; }

{ID} { yylval.str = strdup(yytext); return id; }
\n { yylval.str = strdup(yytext); printf("newline\n"); return newline;}

(\t|\ )

<COML>{COMLCLOSE} { printf("line comment is closed\n"); yy_pop_state();}
<COMB>{COMBCLOSE} { printf("block comment is closed\n"); yy_pop_state();}
<COML,COMB>\n { printf("newline in comments\n");}
<COML,COMB>. { printf("char in comments = %c\n", *yytext);}
<STR>{STRCLOSE} { yy_pop_state();}

. { printf("default = '%c'\n", *yytext); return *yytext; }
%%
