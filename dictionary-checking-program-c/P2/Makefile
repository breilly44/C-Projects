


CC=gcc

CFLAGS=-Wall -g

PATH1=../dict ../my_files/file1 ../my_files/file2 ../my_files/file3
PATH2=/usr/share/dict/words ../my_files/file1 ../my_files/file2 ../my_files/file3
PATH3=/usr/share/dict/words ../testfile ../my_files/file2

SRC=spchk.c

OUT=spchk

all: $(OUT)


$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT)


clean:
	rm -rf $(OUT) *.o spchk.dSYM

test1: $(OUT)
	./$(OUT) $(PATH1)

test2: $(OUT)
	./$(OUT) $(PATH2)

test3: $(OUT)
	./$(OUT) $(PATH3)
