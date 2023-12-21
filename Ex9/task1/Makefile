CC=gcc
CFLAGS=-O2 -Wall -ggdb
CDFLAGS=-ll
OBJ=exec.tab.o lex.yy.o

exec: $(OBJ)
	$(CC) -o $@ $^

exec.tab.c exec.tab.h: exec.y
	bison --defines exec.y

exec.tab.o: exec.tab.c exec.tab.h

lex.yy.c: lexer.l
	flex $^

lex.yy.o: lex.yy.c exec.tab.h

clean:
	rm -f exec.tab.* lex.yy.* exec
