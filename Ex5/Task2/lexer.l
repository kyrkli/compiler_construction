%{
#include <stdio.h>
int lcounter = 0;
int wcounter = 0;
int chcounter = 0;
char** glargv = NULL;
int glargc = 0;
int fcounter = 0;
%}
WHITESPACE [ \t]
WORD [^ \n\t]+
EOL \n
%%
{WHITESPACE} { chcounter++;}	
{WORD} {	
			chcounter += strlen(yytext);
			wcounter++;
		}
{EOL} {	
			lcounter++;
			chcounter++;
		}
.
%%

int yywrap(void){
	if(++fcounter < glargc){
		yyin = fopen(glargv[fcounter], "r");
		return 0;	
	} 
	else {
		printf("Lines: %d\nWords: %d\nChars: %d\n", lcounter, wcounter, chcounter);
		return 1;
	}
}
int main(int argc, char **argv){
	glargv = argv;
	glargc = argc;
	printf("Hello, you can give me filenames and I will count the lines, words and chars numbers for you\n");
	yyin = fopen(argv[1], "r");
	fcounter = 1;
	yylex();
}
