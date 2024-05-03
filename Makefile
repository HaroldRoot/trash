# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -Werror -pedantic -g

# Source files
SRC = shell.c prompt.c builtin.c execute.c utils.c alias.c tokenize.c redirect.c

# Output file
OUT = shell

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
