#ifndef PROJETSTR_DEPRECATED_HASHSET_H
#define PROJETSTR_DEPRECATED_HASHSET_H

#include <stdlib.h>
#include "rsa.h"

typedef struct list {
    int key;
    Key* value;
    struct list* tail;
} DEPRECATED__List;

typedef struct {
    size_t capacity;
    size_t length;
    DEPRECATED__List** tab;
} DEPRECATED__HashTable;

long long int DEPRECATED_hash(unsigned long long key, size_t capacity);
Key* DEPRECATED_listContains(DEPRECATED__List* head, int key);
DEPRECATED__HashTable* DEPRECATED_initHashTable(size_t capacity);
int DEPRECATED_hashTableContains(DEPRECATED__HashTable* hashTable, int key);
Key* DEPRECATED_get(DEPRECATED__HashTable* hashTable, int key);
void DEPRECATED_set(DEPRECATED__HashTable* hashTable, int key, Key* new);
void DEPRECATED_add(DEPRECATED__HashTable* hashTable, int key, Key* value);
void DEPRECATED_freeHashTableListCell(DEPRECATED__List* list);
void DEPRECATED_freeHashTable(DEPRECATED__HashTable* hashTable);

#endif //PROJETSTR_DEPRECATED_HASHSET_H
