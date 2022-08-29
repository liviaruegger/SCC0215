objects = main.c utils.c fixed.c variable.c
valgrind_flags = --tool=memcheck --leak-check=full -s --track-origins=yes --show-leak-kinds=all --show-reachable=yes

all: $(objects)
	gcc $(objects) -o programaTrab -g

run:
	./programaTrab

rem:
	rm *.bin

cpy:
	cp arquivos/antes/* ./

test: all cpy
	./test.sh
	make rem

val:
	valgrind $(valgrind_flags) ./programaTrab

zip: $(objects)
	zip 8586691_12542689.zip $(objects) utils.h fixed.h variable.h Makefile

permission:
	chmod u+r+x test.sh