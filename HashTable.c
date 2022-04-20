#include <stdio.h>
#include <math.h>
#include "HashTable.h"
#include "voting.h"

/*
 * =========== htIk : table de hachage entier vers Key* ===========
 */

size_t htIkFindPosition(HashTable* t, int key) {
    size_t idx = hashInt(key, t->capacity);
    // On cherche tant qu'on ne tombe ni sur une case nulle
    // ni sur une case déjà occupée par la clé désirée.
    while (t->array[idx] != NULL && t->array[idx]->integer != key) {
        idx = (idx + 1) mod t->capacity;
    }
    return idx;
}

int hashInt(int i, size_t capacity) {
    return i mod capacity;
}

/*
 * =========== htKi : table de hachage Key* vers entier ===========
 */

HashTableCell* htKiCreateHashCell(Key* key) {
    HashTableCell *hashCell = malloc(sizeof(HashTableCell));
    if (!hashCell) {
        fprintf(stderr, "[htKiCreateHashCell] Erreur lors de l'allocation :(\n");
        return NULL;
    }

    hashCell->key = copyKey(key);
    hashCell->integer = 0;

    return hashCell;
}

int hashKey(Key* key, size_t size) {
    double A = (sqrt(5) - 1) / 2;
    long x = key->n + key->val;
    return floor(size * (x * A - floor(x * A)));
}

size_t htKiFindPosition(HashTable* t, Key* key) {
    size_t idx = hashKey(key, t->capacity);
    while (t->array[idx] != NULL && !keysEqual(key, t->array[idx]->key)) {
        idx = (idx + 1) mod t->capacity;
    }
    return idx;
}

HashTable* htKiCreateHashTable(CellKey* keys, size_t size) {
    HashTable* newtable = malloc(sizeof(HashTable));
    if (!newtable) {
        fprintf(stderr, "[htKiCreateHashTable] Erreur lors de l'allocation :(\n");
        return NULL;
    }
    newtable->capacity = size;
    newtable->array = calloc(size, sizeof(HashTableCell));
    while (keys) {
        size_t idx = htKiFindPosition(newtable, keys->data);
        if (newtable->array[idx] != NULL) {
            // S'il y a des doublons (ce qui n'est pas censé arriver),
            // on ignore les occurrence d'une clé suivant la première.
            continue;
        } else {
            newtable->array[idx] = htKiCreateHashCell(keys->data);
        }
        keys = keys->next;
    }

    return newtable;
}

int* htKiGetOrNull(HashTable* table, Key* key) {
    size_t idx = htKiFindPosition(table, key);
}

void htKiIncrement(HashTable* table, Key* key) {
    // todo tester
    size_t idx = htKiFindPosition(table, key);
    if (table->array[idx] != NULL) {
        ++(table->array[idx]);
    }
}

Key* htKiArgmax(HashTable* hashTable) {
    Key* argmax = NULL;
    int valmax = -1;

    for (int i = 0; i < hashTable->capacity; ++i) {
        if (valmax < hashTable->array[i]->integer) {
            valmax = hashTable->array[i]->integer;
            freeKey(argmax);
            argmax = copyKey(hashTable->array[i]->key);
        }
    }

    return argmax;
}

/*
 * =========== Général ===========
 */

void deleteHashCell(HashTableCell* hashCell) {
    if (!hashCell) return;
    freeKey(hashCell->key);
    free(hashCell);
}

void deleteHashTable(HashTable* hashTable) {
    if(!hashTable) return;

    for(int i = 0; i < hashTable->capacity; i++){
        deleteHashCell(hashTable->array[i]);
    }
    free(hashTable->array);
    free(hashTable);
}
