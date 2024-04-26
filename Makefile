# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -g

# Source file
SRC = shell.c

# Output file
OUT = shell

all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) -o $@ $<

run: $(OUT)
	./$(OUT)

clean:
	rm -f $(OUT)
