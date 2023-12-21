CC=gcc

CFLAGS=-Wall -O2 -ggdb -Wunused-function

LDFLAGS=-ll

OBJ=lex.yy.o

exec:$(OBJ)
	$(CC) -o $@ $^

lex.yy.c: lexer.l
	flex $^
