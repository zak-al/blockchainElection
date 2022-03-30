#ifndef PROJETSTR_HASHSETKEYTOINT_H
#define PROJETSTR_HASHSETKEYTOINT_H

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

HashCell* create_hashcell(Key* key);

int hash_function(Key* key, int size);

int find_position(HashTableKeyToInt* t, Key* key);

HashTableKeyToInt* create_hashtable(CellKey* keys, int size);

#endif //PROJETSTR_HASHSETKEYTOINT_H
