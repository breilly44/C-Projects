# Define the compiler
CC=gcc

# Define any compile-time flags
CFLAGS=-Wall -g

# Define the source files
SRC=mymalloc.c memgrind.c

# Define the output executable
OUT=memgrind

# Default target
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)

# Clean target to remove executable and object files
clean:
	rm -f $(OUT) *.o


