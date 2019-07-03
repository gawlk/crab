CC = gcc
CFLAGS = -Wall -Wextra -c -ggdb3
EXEC = main
HEADERS = board.h history.h interface.h structs.h

default: exec clean

exec: main.o board.o history.o interface.o
	$(CC) $^ -o $(EXEC)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) $< -o $@

clean:
	-rm -f *.o