objects = main.c utils.c fixed.c variable.c

all: $(objects)
	gcc $(objects) -o main -g

run:
	./main

test: all
	./test.sh

zip: $(objects)
	zip ex01.zip $(objects) utils.h fixed.h variable.h Makefile