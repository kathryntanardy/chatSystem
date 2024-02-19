main: main.c list.o threads.c
	gcc -Wall main.c threads.c list.o -o main -lpthread

clean: 
	rm -f main