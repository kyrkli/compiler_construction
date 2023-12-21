%{
enum{
	EOI = 0,
	OCTAL,
	DEC,
	DECL,
	DECLI
};
	int nr;
%}
%option stack noyywrap
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

DELIM ->

OCTAL octopus{DELIM}({DIGIT17}{DIGIT07}*|0)
DEC dec{DELIM}({DIGIT19}{DIGIT09}*|0)
ID id{DELIM}[A-Z_]+
NUM ({OCTAL}|{DEC})
OP {DELIM}[-*+/]

TERM {NUM}(\n{OP}{NUM})*

DECL int{DELIM}{ID}\n
DECLI {DECL}{DELIM}={TERM}\n
%%
<INITIAL>{COMLOPEN} { yy_push_state(YYSTATE); BEGIN(COML);}
<INITIAL>{COMBOPEN} { yy_push_state(YYSTATE); BEGIN(COMB);}
<INITIAL>{STROPEN} { yy_push_state(YYSTATE); BEGIN(STR);}

<INITIAL,STR>{OCTAL} { nr = strtoul(yytext+9, NULL, 8); printf("octal = %d", nr);}

<COML>{COMLCLOSE} { yy_pop_state();}
<COMB>{COMBCLOSE} { yy_pop_state();}
<STR>{STRCLOSE} { yy_pop_state();}

{DECLI} { printf("DECLI\n"); return DECLI; }
{DECL} { printf("DECL\n"); return DECL;}
<INITIAL,STR>{DEC} { nr = strtoul(yytext+5, NULL, 8); printf("dec = %d", nr);}
<*>. { printf("default\n"); }
%%

int main(int argc, char** argv){
	yyin = fopen(argv[1], "r");
	int token;
	do{
	token = yylex();
	switch(token){
		case DECL:
			printf("DECL is found!\n");
			break;
		case DECLI:
			printf("DECL is found!\n");
			break;
	}
	}while(token != EOI);
}

