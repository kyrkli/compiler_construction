%{
#include <ctype.h>
%}
CHAR [a-zA-Z]
%%
{CHAR} {	
			if(islower(*yytext))
				printf("%c", toupper(*yytext));
			else printf("%c", *yytext);
		}
%%

int yywrap(void){return 1;}
int main(int argc, char **argv){
	yyin = fopen(argv[1], "r");
	yylex();
}
