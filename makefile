CC		= gcc
FLAGS	= -ansi -Wall -pedantic
BIN		= ./bin

prog: $(BIN)/main.o $(BIN)/inpututils.o $(BIN)/charutils.o
	$(CC) $(FLAGS) -g $(BIN)/main.o $(BIN)/inpututils.o -o prog

$(BIN)/main.o: main.c $(BIN)/inpututils.o
	$(CC) $(FLAGS) -c main.c -o $(BIN)/main.o

$(BIN)/inpututils.o: utils/inpututils.c $(BIN)/charutils.o
	$(CC) $(FLAGS) -c utils/inpututils.c -o $(BIN)/inpututils.o

$(BIN)/charutils.o: utils/charutils.c
	$(CC) $(FLAGS) -c utils/charutils.c -o $(BIN)/charutils.o
