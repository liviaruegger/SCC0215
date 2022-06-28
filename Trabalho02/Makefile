objects = main.c utils.c fixed.c variable.c lista.c

all: $(objects)
	gcc $(objects) -o programaTrab -g

run:
	./programaTrab

test: all
	./test.sh

val:
	valgrind --tool=memcheck --leak-check=full -s --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./programaTrab

zip: $(objects)
	zip 8586691_12542689.zip $(objects) utils.h fixed.h variable.h lista.h Makefile
