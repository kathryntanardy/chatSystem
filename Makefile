
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200112L -Werror

all: build

build:
	gcc $(CFLAGS) main.c threads.c list.o -lpthread -o s-talk

main: main.c list.o threads.c
	gcc -Wall main.c threads.c list.o -o main -lpthread
main.c threads.c list.o -o main -lpthread

valgrind: build
	valgrind --leak-check=full ./main

mainonly: main.c threads.c
	gcc -Wall main.c -o main

clean: 
	rm -f s-talk