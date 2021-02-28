INC = includes/
LIB = lib/
OBJ = obj/
SLIB = srclib/
SRC = src/

CC = gcc
CFLAGS = -Wall -Iincludes

all: server_test

.PHONY: clean

server_test: $(OBJ)server_test.o $(LIB)libgenericserver.a
	$(CC) -o $@.exe $< -pthread -L$(LIB) -lgenericserver

# Library #
$(LIB)libgenericserver.a: $(OBJ)connection.o $(OBJ)utils.o
	ar rcs $@ $^

# Compile #
$(OBJ)server_test.o: server_test.c $(INC)connection.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)connection.o: $(SLIB)connection.c $(INC)connection.h $(INC)utils.h
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJ)utils.o: $(SLIB)utils.c $(INC)utils.h
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm -rf *.o $(OBJ)*.o

clean_all:
	rm -rf *.o $(OBJ)*.o *.exe *.a $(LIB)*.a
