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

size_t htIkFindPosition(const HashTable* t, int key);

void htIkAdd(const HashTable* table, int key, const Key* value);

int htIkExists(HashTable* table, int key);

Key* htIkGetOrNull(HashTable* table, int key);

HashTable* htIkConstruct(size_t capacity);

HashTableCell* htIkCreateHashCell(int key);

size_t hashInt(int i, size_t capacity);

void htKiAdd(HashTable* table, Key* key, int value);

int* htKiGetOrNull(HashTable* table, Key* key);

int htKiExists(const HashTable* table, const Key* key);

void htKiIncrement(HashTable* table, Key* key);

int hashKey(const Key* key, size_t capacity);

size_t htKiFindPosition(const HashTable* t, const Key* key);

HashTableCell* htKiCreateHashCell(Key* key);

HashTable* htKiCreateHashTable(CellKey* keys, size_t size);

Key* htKiArgmax(HashTable* hashTable);

void deleteHashTable(HashTable* hashTable);

void deleteHashCell(HashTableCell* hashCell);

#endif //PROJETSTR_HASHTABLE_H
