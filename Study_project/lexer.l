%{
#include "stack.h"
#include "ast.h"
#include "exec.tab.h"
%}
%option stack noyywrap noinput nounput yylineno
%x COML COMB
COMLOPEN @@
COMLCLOSE \n

COMBOPEN @
COMBCLOSE #\n

STRING str\(.*\)end

DIGIT09 [0-9]
DIGIT19 [1-9]
DIGIT07 [0-7] 
DIGIT17 [1-7]
DIGIT0F [0-9a-fA-F]
DIGIT1F [1-9a-fA-F]

OCTAL [+-]?{DIGIT17}{DIGIT07}*|0
HEX [+-]?{DIGIT1F}{DIGIT0F}*|0
DEC [+-]?{DIGIT19}{DIGIT09}*|0
ID [a-zA-Z_]+{DIGIT09}*
CH '.' 

OCTALDEF octopus->{OCTAL}
HEXDEF hex->{HEX}
DECDEF {DEC}

HEXREAL hex->{HEX}.(0)*{HEX}
DECREAL {DEC}.(0)*{DEC}

%%
{COMLOPEN} { printf("line comment is open\n"); yy_push_state(YYSTATE); BEGIN(COML);}
{COMBOPEN} { printf("block comment is open\n"); yy_push_state(YYSTATE); BEGIN(COMB);}

{OCTALDEF} { yylval.svar.gval.int_val = strtoul(yytext+9, NULL, 8); 
			yylval.svar.type = _int;
			return num;}
{DECDEF} { 	yylval.svar.gval.int_val = strtoul(yytext, NULL, 10); 
			yylval.svar.type = _int;
			printf("int = %d\n", yylval.svar.gval.int_val);
			return num; }
{HEXDEF} { 	yylval.svar.gval.int_val = strtoul(yytext+5, NULL, 16);
			yylval.svar.type = _int; 
			return num; }

{HEXREAL} { yytext[3] = '0'; yytext[4] = 'x'; 
			yylval.svar.gval.double_val = strtod(yytext+3, NULL); 
			yylval.svar.type = _double; 
			return num;}
{DECREAL} { printf("yytext = %s", yytext);
			yylval.svar.gval.double_val = strtod(yytext, NULL); 
			yylval.svar.type = _double;
			printf("double = %f\n", yylval.svar.gval.double_val);
			return num;}
{STRING} {	char new_string[100];
			//cut str( and )end
			strncpy(new_string, yytext + 4, strlen(yytext) - 8);
			new_string[strlen(yytext) - 8] = '\0';  // Null-terminate the new string
			yylval.svar.gval.charptr_val = strdup(new_string);
			yylval.svar.type = _charptr;
			return str;}	


intp { yylval.type = _intptr; return type; }
realp { yylval.type = _doubleptr; return type; }
boolp { yylval.type = _boolptr; return type; }

char { yylval.type = _char; return type; }
int { yylval.type = _int; return type; }
real { yylval.type = _double; return type; }
bool { yylval.type = _bool; return type; }
string { yylval.type = _charptr; return type; }

or { printf("or\n"); return or; }
and { printf("and\n"); return and; }
xor { printf("xor\n"); return xor; }
not { printf("not\n"); return not; }

mod { printf("mod\n"); return mod; }

nl { printf("nl\n");
	yylval.svar.type = _char; 
	yylval.svar.gval.char_val = '\n';	
	return nl; }
-> { printf("arrow\n"); return arrow; }
p: { printf("p:\n"); return pp;}
el: { printf("el:\n"); return el;}

SIZE { printf("SIZE\n"); return size;}
RANDOM { printf("RANDOM\n"); return _random;}
SCANF { printf("SCANF\n"); return _scanf; }
CALL { printf("CALL\n"); return call;}
FUNC { printf("FUNC\n"); return func;}
FOR { printf("FOR\n"); return _for; }
PRINT { printf("PRINT\n"); return print;}

WHILE { printf("WHILE\n"); return _while; }
IF { printf("IF\n"); return _if; }
ELSE { printf("ELSE\n"); return _else; }

TRUE  { printf("TRUE\n");
		yylval.svar.type = _bool; 
		yylval.svar.gval.int_val = 1;
		return boolean;}
FALSE { printf("FALSE\n"); 
		yylval.svar.type = _bool; 
		yylval.svar.gval.int_val = 0;
		return boolean;}

{CH}  { yylval.svar.type = _char;
		printf("Hello its char\n");
		yylval.svar.gval.char_val = yytext[1]; 
		return ch;}

{ID} { yylval.svar.id = strdup(yytext); printf("id = %s\n", yytext); return id; }
\n { yylval.str = strdup(yytext); printf("newline\n"); return newline;}

(\t|\ )

<COML>{COMLCLOSE} { printf("line comment is closed\n"); yy_pop_state();}
<COMB>{COMBCLOSE} { printf("block comment is closed\n"); yy_pop_state();}
<COML,COMB>\n { printf("newline in comments\n");}
<COML,COMB>. { printf("char in comments = %c\n", *yytext);}

. { printf("default = '%c'\n", *yytext); return *yytext; }
%%
