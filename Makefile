OBJ = obj/
SLIB = srclib/
INC = includes/
SRC = src/

CC = gcc
CFLAGS = -Wall -Iincludes

all: server_test

.PHONY: clean

server_test: $(OBJ)server_test.o $(OBJ)connection.o $(OBJ)utils.o
	$(CC) -o $@.exe $^ -pthread


$(OBJ)server_test.o: server_test.c $(INC)connection.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)connection.o: $(SLIB)connection.c $(INC)connection.h $(INC)utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)utils.o: $(SLIB)utils.c $(INC)utils.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o $(OBJ)*.o

clean_all:
	rm -rf *.o $(OBJ)*.o *.exe
