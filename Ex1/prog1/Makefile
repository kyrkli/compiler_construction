CC=gcc
CFLAGS=-Wall -O2
LDFLAGS=-lpthread -r
OBJ=main.o check.o

hello: $(OBJ)
	$(CC) -o  $@ $^

main.o: main.c numbers.txt
	$(CC) $(CFLAGS) $(LDFLAGS) main.c -o main.o

numbers.txt:
	seq 1 7 1000 > numbers.txt

clean: 
	rm -f $(OBJ) hello numbers.txt

