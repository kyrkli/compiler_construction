CC=gcc
CFLAGS= -O2 -Wall -ggdb
OBJ = struc.o main.o ringQueue.o

exec: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(OBJ) exec

struc.o: struc.h
ringQueue.o : ringQueue.h
