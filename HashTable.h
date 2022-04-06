#ifndef PROJETSTR_HASHTABLE_H
#define PROJETSTR_HASHTABLE_H

#include <stdlib.h>
#include "rsa.h"
#include "voting.h"

typedef struct {
    Key* key;
    int integer;
} HashTableCell;

typedef struct {
    size_t capacity;
    HashTableCell** array;
} HashTable;

void htIkAdd(HashTable* table, int key, Key* value);
// todo ne pas forcément renvoyer de pointeur vers key pour éviter d'avoir à libérer si on ne veut que vérifier l'existence
Key* htIkGetOrNull(HashTable* table, int key);
void htIkConstruct(size_t capacity);
int hashInt(int i, size_t capacity);

void htKiAdd(HashTable* table, Key* key, int value);
int* htKiGetOrNull(HashTable* table, Key* key);
void htKiIncrement(HashTable* table, Key* key);
int hashKey(Key* key, size_t capacity);
size_t htKiFindPosition(HashTable* hashTable, Key* key);
HashTableCell* htKiCreateHashCell(Key* key);
HashTable* htKiCreateHashTable(CellKey* keys, size_t size);
Key* htKiArgmax(HashTable* hashTable);

void deleteHashTable(HashTable* hashTable);
void deleteHashCell(HashTableCell* hashCell);

#endif //PROJETSTR_HASHTABLE_H
