CC = gcc
FLAGS = -ansi -Wall -pedantic

SRC_DIR = src
OBJ_DIR = bin

# List source files recursively
FILES = $(shell find $(SRC_DIR) -name "*.c") main.c

# Generate object file paths based on relative source paths
OBJ_FILES = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(FILES))

prog: $(OBJ_FILES)
	$(CC) $(FLAGS) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(shell dirname "$@")
	@touch "$@"
	$(CC) $(FLAGS) -c "$<" -o $@

