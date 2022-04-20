#ifndef PROJETSTR_DEPRECATED_HASHSET_H
#define PROJETSTR_DEPRECATED_HASHSET_H

#include <stdlib.h>
#include "rsa.h"

typedef struct list {
    int key;
    Key* value;
    struct list* tail;
} __List;

typedef struct {
    size_t capacity;
    size_t length;
    __List** tab;
} __HashTable;

long long int DEPRECATED_hash(unsigned long long key, size_t capacity);
Key* DEPRECATED_listContains(__List* head, int key);
__HashTable* DEPRECATED_initHashTable(size_t capacity);
int DEPRECATED_hashTableContains(__HashTable* hashTable, int key);
Key* DEPRECATED_get(__HashTable* hashTable, int key);
void set(__HashTable* hashTable, int key, Key* new);
void add(__HashTable* hashTable, int key, Key* value);
void freeHashTableListCell(__List* list);
void freeHashTable(__HashTable* hashTable);

#endif //PROJETSTR_DEPRECATED_HASHSET_H
