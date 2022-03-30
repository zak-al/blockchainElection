//
// Created by Zakarie Aloui on 26/03/2022.
//

#ifndef PROJETSTR_HASHSET_H
#define PROJETSTR_HASHSET_H

#include <stdlib.h>
#include "rsa.h"

typedef struct list {
    int key;
    Key* value;
    struct list* tail;
} List;

typedef struct {
    size_t capacity;
    size_t length;
    List** tab;
} HashTable;

long long int hash(unsigned long long key, size_t capacity);
Key* listContains(List* head, int key);
HashTable* initHashTable(size_t capacity);
int hashTableContains(HashTable* hashTable, int key);
Key* get(HashTable* hashTable, int key);
void set(HashTable* hashTable, int key, Key* new);
void add(HashTable* hashTable, int key, Key* value);
void freeHashTableListCell(List* list);
void freeHashTable(HashTable* hashTable);

#endif //PROJETSTR_HASHSET_H
