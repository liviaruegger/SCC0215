objects = main.c utils.c btree.c data.c func.c

all: $(objects)
	gcc $(objects) -o programaTrab -g

run:
	./programaTrab

test: all
	./test.sh

val:
	valgrind --tool=memcheck --leak-check=full -s --track-origins=yes --show-leak-kinds=all --show-reachable=yes ./programaTrab

zip: $(objects)
	zip 8586691_12542689.zip $(objects) utils.h btree.h data.h func.h Makefile
