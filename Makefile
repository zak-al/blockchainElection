tests: tests.o voting.o HashTable.o rsa.o primalite.o HashTable.o Protected.o Signature.o blockchain.o
	gcc tests.o voting.o rsa.o hashset.o primalite.o HashTable.o Protected.o Signature.o blockchain.o -o tests -lssl -lcrypto

tests.o: tests.c voting.c
	gcc -Wall -c tests.c

primalite.o: primalite.c primalite.h
	gcc -Wall -c primalite.c

rsa.o: rsa.c rsa.h primalite.c primalite.h
	gcc -Wall -c rsa.c

voting.o: voting.c voting.h rsa.c rsa.h hashset.c hashset.h
	gcc -Wall -c voting.c

HashTable.o: HashTable.c HashTable.h
	gcc -Wall -c HashTable.c

Protected.o: Protected.c Protected.h
	gcc -Wall -c Protected.c

Signature.o: Signature.c Signature.h
	gcc -Wall -c Signature.c

blockchain.o: blockchain.c blockchain.h
	gcc -Wall -c blockchain.c

clean:
	rm -rf *.o
