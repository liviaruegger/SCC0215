objects = main.c utils.c fixed.c variable.c

all: $(objects)
	gcc $(objects) -o main

run:
	./main

test: all
	./test.sh

zip: $(objects)
	zip 8586691_12542689.zip $(objects) utils.h fixed.h variable.h Makefile