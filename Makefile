main: main.o primalite.o
	gcc main.o primalite.o -o main

main.o: main.c
	gcc -Wall -c main.c

primalite.o: primalite.c primalite.h
	gcc -Wall -c primalite.c

clean:
	rm -rf *.o
