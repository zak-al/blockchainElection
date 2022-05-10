tests: tests.o voting.o HashTable.o rsa.o primalite.o Protected.o Signature.o blockchain.o Arborescence.o Simulation.o Hash.o
	gcc -Wall tests.o voting.o rsa.o primalite.o HashTable.o Protected.o Signature.o blockchain.o Arborescence.o Simulation.o Hash.o -o tests -lssl -lcrypto

tests.o: tests.c voting.c
	gcc -Wall -c tests.c

primalite.o: primalite.c primalite.h
	gcc -Wall -c primalite.c

rsa.o: rsa.c rsa.h primalite.c primalite.h
	gcc -Wall -c rsa.c

voting.o: voting.c voting.h rsa.c rsa.h
	gcc -Wall -c voting.c

HashTable.o: HashTable.c HashTable.h
	gcc -Wall -c HashTable.c

Protected.o: Protected.c Protected.h
	gcc -Wall -c Protected.c

Signature.o: Signature.c Signature.h
	gcc -Wall -c Signature.c

blockchain.o: blockchain.c blockchain.h
	gcc -Wall -c blockchain.c

Arborescence.o: Arborescence.c Arborescence.h
	gcc -Wall -c Arborescence.c

Simulation.o: Simulation.c Simulation.h
	gcc -Wall -c Simulation.c

Hash.o: Hash.c Hash.h
	gcc -Wall -c Hash.c

clean:
	rm -rf *.o
