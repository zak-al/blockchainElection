#include <stdio.h>
#include "DEPRECATED_hashset.h"

long long int DEPRECATED_hash(unsigned long long key, size_t capacity) {
    return key mod capacity;
}

Key *DEPRECATED_listContains(DEPRECATED__List *head, int key) {
    while (head) {
        if (key == head->key) {
            return copyKey(head->value);
        }
        head = head->tail;
    }

    return NULL;
}

DEPRECATED__HashTable *DEPRECATED_initHashTable(size_t capacity) {
    DEPRECATED__HashTable *hashTable = malloc(sizeof(DEPRECATED__HashTable));

    if (!hashTable) {
        fprintf(stderr, "[DEPRECATED_initHashTable] Erreur lors de l'allocation du tableau :(\n");
        return NULL;
    }

    hashTable->length = 0;
    hashTable->capacity = capacity;
    hashTable->tab = calloc(capacity, sizeof(DEPRECATED__List *));

    if (!hashTable->tab) {
        fprintf(stderr, "[DEPRECATED_initHashTable] Erreur lors de l'allocation du tableau :(\n");
        free(hashTable);
        return NULL;
    }

    return hashTable;
}

int DEPRECATED_hashTableContains(DEPRECATED__HashTable *hashTable, int key) {
    long long h = DEPRECATED_hash(key, hashTable->capacity);
    return DEPRECATED_listContains(hashTable->tab[h], key) != NULL;
}

Key *DEPRECATED_get(DEPRECATED__HashTable *hashTable, int key) {
    long long h = DEPRECATED_hash(key, hashTable->capacity);
    return DEPRECATED_listContains(hashTable->tab[h], key);
}

void DEPRECATED_set(DEPRECATED__HashTable *hashTable, int key, Key *new) {
    long long h = DEPRECATED_hash(key, hashTable->capacity);
    DEPRECATED__List *l = hashTable->tab[h];
    while (l) {
        if (l->key == key) {
            freeKey(l->value);
            l->value = new != NULL ? copyKey(new) : NULL;
            break;
        }
        l = l->tail;
    }
}

void DEPRECATED_add(DEPRECATED__HashTable *hashTable, int key, Key *value) {
    long long h = DEPRECATED_hash(key, hashTable->capacity);

    DEPRECATED__List *newCell = malloc(sizeof(DEPRECATED__List));
    if (!newCell) {
        fprintf(stderr, "[DEPRECATED_add] Erreur lors de l'allocation de newCell :(\n");
        return;
    }

    newCell->value = value != NULL ? copyKey(value) : NULL;
    newCell->key = key;
    newCell->tail = hashTable->tab[h];
    hashTable->tab[h] = newCell;
}

void DEPRECATED_freeHashTableListCell(DEPRECATED__List *list) {
    if (!list) return;
    freeKey(list->value);
    free(list);
}

void DEPRECATED_freeHashTable(DEPRECATED__HashTable *hashTable) {
    if (!hashTable) return;

    for (int i = 0; i < hashTable->capacity; ++i) {
        DEPRECATED__List *l = hashTable->tab[i];
        while (l) {
            DEPRECATED__List *tail = l->tail;
            DEPRECATED_freeHashTableListCell(l);
            l = tail;
        }
    }

    free(hashTable->tab);
    free(hashTable);
}