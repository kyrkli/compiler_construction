%{
#include "stack.h"
#include "ast.h"
#include "exec.tab.h"
char tmpstring[200];
%}
%option stack noyywrap noinput nounput yylineno
%x COML COMB STR
COMLOPEN @@
COMLCLOSE \n

COMBOPEN @
COMBCLOSE #\n

STRING (^(\)end))*
STROP str[(]
STRCL [)]end


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
{COMLOPEN} { yy_push_state(YYSTATE); BEGIN(COML);}
{COMBOPEN} { yy_push_state(YYSTATE); BEGIN(COMB);}
{STROP} { yy_push_state(YYSTATE); BEGIN(STR); }

{OCTALDEF} { yylval.svar.gval.int_val = strtoul(yytext+9, NULL, 8); 
			yylval.svar.type = _int;
			return num;}
{DECDEF} { 	yylval.svar.gval.int_val = strtoul(yytext, NULL, 10); 
			yylval.svar.type = _int;
			return num; }
{HEXDEF} { 	yylval.svar.gval.int_val = strtoul(yytext+5, NULL, 16);
			yylval.svar.type = _int; 
			return num; }

{HEXREAL} { yytext[3] = '0'; yytext[4] = 'x'; 
			yylval.svar.gval.double_val = strtod(yytext+3, NULL); 
			yylval.svar.type = _double; 
			return num;}
{DECREAL} {	yylval.svar.gval.double_val = strtod(yytext, NULL); 
			yylval.svar.type = _double;
			return num;}

intp { yylval.type = _intptr; return type; }
realp { yylval.type = _doubleptr; return type; }
boolp { yylval.type = _boolptr; return type; }

void { yylval.type = _void; return type; }
char { yylval.type = _char; return type; }
int { yylval.type = _int; return type; }
real { yylval.type = _double; return type; }
bool { yylval.type = _bool; return type; }
string { yylval.type = _charptr; return type; }

or { return or; }
and { return and; }
xor { return xor; }
not { return not; }

mod { return mod; }

nl { yylval.svar.type = _char; 
	yylval.svar.gval.char_val = '\n';	
	return nl; }
-> { return arrow; }
p: { return pp;}
el: { return el;}
D { return d;}
T { return t;}

SIZE { return size;}
RANDOM { return _random;}
SCANF { return _scanf; }
CALL { return call;}
FUNC { return func;}
FOR { return _for; }
PRINT { return print;}

WHILE { return _while; }
IF { return _if; }
ELSE { return _else; }

TRUE  { yylval.svar.type = _bool; 
		yylval.svar.gval.int_val = 1;
		return boolean;}
FALSE { yylval.svar.type = _bool; 
		yylval.svar.gval.int_val = 0;
		return boolean;}

{CH}  { yylval.svar.type = _char;
		yylval.svar.gval.char_val = yytext[1]; 
		return ch;}

{ID} { yylval.svar.id = strdup(yytext); return id; }
\n { yylval.str = strdup(yytext); return newline;}

(\t|\ )

<STR>{STRCL}  { yylval.svar.gval.charptr_val = strdup(tmpstring);
				yylval.svar.type = _charptr;
			   	yy_pop_state();
				strcpy(tmpstring, "");
				return str;}
<STR>.  { strncat(tmpstring, &yytext[0], 1); }
<COML>{COMLCLOSE} { yy_pop_state();}
<COMB>{COMBCLOSE} { yy_pop_state();}

. { return *yytext; }
%%
