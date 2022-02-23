main: main.o partie1.o
	gcc main.o partie1.o -o main

main.o: main.c
	gcc -Wall -c main.c

partie1.o: partie1.c partie1.h
	gcc -Wall -c partie1.c

clean:
	rm -rf *.o
