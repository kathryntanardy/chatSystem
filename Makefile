main: main.c list.o threads.c
	gcc -Wall main.c threads.c list.o -o main -lpthread

mainonly: main.c threads.c
	gcc -Wall main.c -o main

clean: 
	rm -f main