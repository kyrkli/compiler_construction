%{
//includes
#include "exec.tab.h"
%}

%option noyywrap noinput nounput

TRUE TRUE
FALSE FALSE
AND and
OR  or
XOR xor
NOT not

%%
{TRUE} { return _true; }
{FALSE} { return _false; }
{AND} { return _and; }
{OR} { return _or; }
{XOR} { return _xor; }
{NOT} { return _not; }
(\ |\n)
. { return *yytext; }
%%
//c code
