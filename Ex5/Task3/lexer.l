%{
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int sum_evennum = 0;
int sum_oddnum = 0;

%}
%x OCTAL HEXAD
END END
EVENNUM ([1-9][0-9]*)?[02468]
ODDNUM ([1-9][0-9]*)?[13579]
EVENID ([a-zA-Z][a-zA-Z])+
ODDID [a-zA-Z](([a-zA-Z][a-zA-Z])+)?
BLANK [ ]
NEWLINE \n
OCT [1-7][0-7]*|0
HEX [1-9a-fA-F][0-9a-fA-F]*|0

%option stack noyywrap
%%
<*>"/x" { yy_push_state(YYSTATE); BEGIN(HEXAD);}
<HEXAD>"x/" { yy_pop_state();}
<HEXAD>{HEX} {	
				int hexToDec = strtol(yytext, NULL, 16);
				printf("%d", hexToDec);	
				if(hexToDec%2){
					sum_oddnum += hexToDec;
					return 2;
				}
				else{
					sum_evennum += hexToDec;
					return 1;
				}
			}

<*>"/o" { yy_push_state(YYSTATE); BEGIN(OCTAL);}
<OCTAL>"o/"	{ yy_pop_state();}
<OCTAL>{OCT} {	
				int octToDec = strtol(yytext, NULL, 8);
				printf("%d", octToDec);	
				if(octToDec%2){
					sum_oddnum += octToDec;
					return 2;
				}
				else{
					sum_evennum += octToDec;
					return 1;
				}
			}

<*>{END} {
		return 3;
		}
{EVENNUM} {
			sum_evennum += atoi(yytext);
			return 1;
			}
{ODDNUM} {		
			sum_oddnum += atoi(yytext);
			return 2;
			}
<*>{EVENID} {
			for(int i = 0; i < strlen(yytext); i++)
				if(isupper(yytext[i]))
					yytext[i] = tolower(yytext[i]);	
			ECHO;	
			}
<*>{ODDID}	{
			for(int i = 0; i < strlen(yytext); i++)
				if(islower(yytext[i]))
					yytext[i] = toupper(yytext[i]);
			ECHO;
		}
<*>{BLANK} {
			ECHO;
		}
<*>{NEWLINE} {
			ECHO;
			}

<*>.
%%


int main(int argc, char **argv){
	yyin = fopen(argv[1], "r");
	int val;
	do {
		val = yylex();
		
		switch(val){
		case 0:
				break;
		case 1:
				printf("\nSum evennum = %d\n", sum_evennum);
				break;
		case 2:	
				printf("\nSum oddnum = %d\n", sum_oddnum);
				break;
		case 3:
				printf("\nEND encountered\n");
				val = 0;
				break;
		}
	} while(val != 0);

	printf("\nSum evennum = %d; Sum oddnum = %d\n", sum_evennum, sum_oddnum);	
	return 0;
}
