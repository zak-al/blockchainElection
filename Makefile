main: main.o voting.o rsa.o primalite.o
	gcc main.o voting.o rsa.o primalite.o -o main

main.o: main.c
	gcc -Wall -c main.c

primalite.o: primalite.c primalite.h
	gcc -Wall -c primalite.c

voting.o: voting.c voting.h
	gcc -Wall -c voting.c

rsa.o: rsa.c rsa.h
	gcc -Wall -c rsa.c

clean:
	rm -rf *.o main