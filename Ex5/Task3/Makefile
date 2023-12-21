CC=gcc

CFLAGS=-O2 -Wall -ggdb -Wunused-function
LDFLAGS=-ll

OBJ=lex.yy.o

exec: $(OBJ)
	$(CC) -o $@ $^

lex.yy.c: lexer.l
	flex $^

