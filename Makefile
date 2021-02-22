CC = gcc -g
CFLAGS = -Wall

all: server_test clean

.PHONY: clean

server_test: server_test.o connection.o
	$(CC) -o $@.exe $^ -pthread


server_test.o: server_test.c connection.h
	$(CC) $(CFLAGS) -c $< -o $@

connection.o: connection.c connection.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o

clean_all:
	rm -rf *.o *.exe
