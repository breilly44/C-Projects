CC=gcc

SRC=mysh.c

OUT=mysh

all: $(OUT)

S(OUT): $(SRC)
	$(CC) $(SRC) -o $(OUT)

intr: $(OUT)
	./mysh

batch: $(OUT)
	./mysh foo.sh
clean:
	rm -f $(OUT)
