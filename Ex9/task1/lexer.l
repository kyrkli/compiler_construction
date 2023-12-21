%{
#include "exec.tab.h"
%}

NUM [1-9][0-9]*|0
OP [+*/-]
ADD [+]
MIN [-]
MULT [*]
DIV [/]
ID [a-zA-Z_]+
%option noyywrap noinput nounput

%%
{NUM} { yylval.num = atoi(yytext); return num; }
{ADD} { yylval.ch = *yytext; return add;}
{MIN} { yylval.ch = *yytext; return min;}
{MULT} { yylval.ch = *yytext; return mult;}
{DIV} { yylval.ch = *yytext; return _div;}
{ID} { printf("id = %s\n", yytext); yylval.id = yytext; return id;}
(\ |\n)
. { return *yytext; }
%%
