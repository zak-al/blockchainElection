main: main.o primalite.o
	gcc main.o primalite.o -o main

main.o: main.c
	gcc -Wall -c main.c

tests: tests.o voting.o hashset.o rsa.o primalite.o
	gcc tests.o voting.o rsa.o hashset.o primalite.o -o tests

tests.o: tests.c voting.c
	gcc -Wall -c tests.c

primalite.o: primalite.c primalite.h
	gcc -Wall -c primalite.c

rsa.o: rsa.c rsa.h primalite.c primalite.h
	gcc -Wall -c rsa.c

voting.o: voting.c voting.h rsa.c rsa.h hashset.c hashset.h
	gcc -Wall -c voting.c

hashset.o: hashset.c hashset.h
	gcc -Wall -c hashset.c

clean:
	rm -rf *.o
