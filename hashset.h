#ifndef PROJETSTR_HASHSET_H
#define PROJETSTR_HASHSET_H

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

long long int hash(unsigned long long key, size_t capacity);
Key* listContains(__List* head, int key);
__HashTable* initHashTable(size_t capacity);
int hashTableContains(__HashTable* hashTable, int key);
Key* get(__HashTable* hashTable, int key);
void set(__HashTable* hashTable, int key, Key* new);
void add(__HashTable* hashTable, int key, Key* value);
void freeHashTableListCell(__List* list);
void freeHashTable(__HashTable* hashTable);

#endif //PROJETSTR_HASHSET_H
