CC=gcc
CFLAGS=-O2 -Wall -ggdb
CDFLAGS=-ll -Wunused-function 
OBJ=exec.tab.o lex.yy.o variable_handling.o stack.o ast.o queue.o

exec: $(OBJ)
	$(CC) -o $@ $^ -lm

exec.tab.c exec.tab.h: exec.y
	bison --defines exec.y -Wcounterexamples

exec.tab.o: exec.tab.c exec.tab.h

lex.yy.c: lexer.l
	flex $^

lex.yy.o: lex.yy.c exec.tab.h

clean:
	rm -f exec.tab.* lex.yy.* stack.o variable_handling.o ast.o exec queue.o
