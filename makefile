CC		= gcc
FLAGS	= -ansi -Wall -pedantic
BIN		= ./bin

prog: $(BIN)/main.o
	$(CC) $(FLAGS) -g $(BIN)/main.o -o prog

$(BIN)/main.o: main.c
	$(CC) $(FLAGS) -c main.c -o "$(BIN)/main.o"
