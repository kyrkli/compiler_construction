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

TYPEID (int | char | float | double)
PTRTYPE *

OCTAL {DIGIT17}{DIGIT07}*|0
DEC {DIGIT19}{DIGIT09}*|0
HEX {DIGIT1F}{DIGIT0F}*|0
ID [a-zA-Z_]+{DIGIT09}*

DELIM ->

OCTALDEF octopus{DELIM}{OCTAL}
DECDEF {DELIM}{DEC}
HEXDEF hex{DELIM}{HEX}
IDDEF id{DELIM}{ID}

NUM ({OCTAL}|{DEC}|{HEXDEF})
OP {DELIM}[-*+/]

TERM {NUM}(\n{OP}{NUM})*

DECL int{DELIM}{IDDEF}\n
DECLI {DECL}{DELIM}={TERM}
%%
<INITIAL>{COMLOPEN} { yy_push_state(YYSTATE); BEGIN(COML);}
<INITIAL>{COMBOPEN} { yy_push_state(YYSTATE); BEGIN(COMB);}
<INITIAL>{STROPEN} { yy_push_state(YYSTATE); BEGIN(STR);}

<INITIAL,STR>{OCTAL} { yylval.num = strtoul(yytext+9, NULL, 8);}

<COML>{COMLCLOSE} { yy_pop_state();}
<COMB>{COMBCLOSE} { yy_pop_state();}
<STR>{STRCLOSE} { yy_pop_state();}

{DECLI} { printf("DECLI\n"); return DECLI; }
{DECL} { printf("DECL\n"); return DECL;}
<INITIAL,STR>{DEC} { yylval.num = strtoul(yytext+5, NULL, 8);}
<*>. { printf("default\n"); }
%%
