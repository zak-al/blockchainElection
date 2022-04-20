#ifndef PROJETSTR_DEPRECATED_HASHSETKEYTOINT_H
#define PROJETSTR_DEPRECATED_HASHSETKEYTOINT_H

#include "stdio.h"
#include "rsa.h"
#include "voting.h"

typedef struct {
    Key* key;
    int val;
} HashCell;

typedef struct hashtable {
    HashCell** tab;
    int size;
} HashTableKeyToInt;

int hash_function(Key* key, int size);

size_t find_position(HashTableKeyToInt* t, Key* key);

HashTableKeyToInt* create_hashtable(CellKey* keys, int size);

void delete_hashtable(HashTableKeyToInt* t);
void freeHashCell(HashCell *h);

#endif //PROJETSTR_DEPRECATED_HASHSETKEYTOINT_H