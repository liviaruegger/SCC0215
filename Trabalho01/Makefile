objects = main.c utils.c fixed.c variable.c

all: $(objects)
	gcc $(objects) -o programaTrab -g

run:
	./programaTrab

rem:
	rm *.bin
	rm *.csv

cpy:
	cp arquivos/* ./

test: all cpy
	./test.sh
	make rem

zip: $(objects)
	zip 8586691_12542689.zip $(objects) utils.h fixed.h variable.h Makefile

permission:
	chmod u+r+x test.sh