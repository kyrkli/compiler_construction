%{
#include "exec.tab.h"
%}

NUM [1-9][0-9]*|0
OP [+*/-]
ID [a-zA-Z_]+
%option noyywrap noinput nounput

%%
{NUM} { yylval.num = atoi(yytext); return num; }
{OP} { yylval.ch = *yytext; return op;}
{ID} { printf("id = %s\n", yytext); yylval.id = yytext; return id;}
(\ |\n)
. { return *yytext; }
%%
