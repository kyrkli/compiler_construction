%{
#include "exec.tab.h"

%}
%option stack noyywrap noinput nounput
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
ID [a-zA-Z_]+{DIGIT09}*

OCTALDEF octopus->{OCTAL}
HEXDEF hex->{HEX}
DECDEF {DIGIT19}{DIGIT09}*|0

NUM ({OCTAL}|{DEC}|{HEXDEF})

%%
{COMLOPEN} { yy_push_state(YYSTATE); BEGIN(COML);}
{COMBOPEN} { yy_push_state(YYSTATE); BEGIN(COMB);}
{STROPEN} { yy_push_state(YYSTATE); BEGIN(STR);}

{OCTALDEF} { yylval.num = strtoul(yytext+9, NULL, 8); return num;}
{DECDEF} { yylval.num = strtoul(yytext, NULL, 10); return num; }
{HEXDEF} { yylval.num = strtoul(yytext+5, NULL, 16); return num; }

char { yylval.type = _char; return type; }
int { yylval.type = _int; return type; }
float { yylval.type = _float; return type; }
double { yylval.type = _double; return type; }

{ID} { yylval.id = strdup(yytext); return id; }
\n { yylval.id = strdup(yytext); return newline;}

<COML>{COMLCLOSE} { yy_pop_state();}
<COMB>{COMBCLOSE} { yy_pop_state();}
<STR>{STRCLOSE} { yy_pop_state();}

<*>. { printf("default = %c\n", *yytext); return *yytext; }
%%
