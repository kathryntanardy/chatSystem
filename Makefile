CFLAG = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200112L -Werror

all:
	build
	
build: 
	gcc $(CFLAGS) main.c threads.c list.o -o main -lpthread

run: build
	./main

valgrind: build
	valgrind --leak-check=full ./main
# main: main.c list.o threads.c
# 	gcc -Wall main.c threads.c list.o -o main -lpthread

clean: 
	rm -f main