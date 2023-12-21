%{
#include <string.h>
%}
%option stack noyywrap
%x DQUOTES
DEC [1-9][0-9]*|0
DQUOT ["]
DQUOT2 ["]["]
NL [\\][n]
STRC [.]
%%
{DEC} { printf("Number found: %d\n", atoi(yytext));}
{DQUOT} {	printf("DQUOT found and started\n");
			BEGIN(DQUOTES); }
<DQUOTES>{DQUOT} { 	printf("DQUOT found and ended\n");
					BEGIN(INITIAL); }
<DQUOTES>{STRC} {
				printf("String found: '%s'\n", yytext);}
<*>.
%%

int main(int argc, char** argv){
	yyin = fopen(argv[1], "r");
	yylex();
}
