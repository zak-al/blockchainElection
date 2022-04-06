#include <stdio.h>
#include "hashset.h"

long long int hash(unsigned long long key, size_t capacity) {
    return key mod capacity;
}

Key *listContains(__List *head, int key) {
    while (head) {
        if (key == head->key) {
            return copyKey(head->value);
        }
        head = head->tail;
    }

    return NULL;
}

__HashTable *initHashTable(size_t capacity) {
    __HashTable *hashTable = malloc(sizeof(__HashTable));

    if (!hashTable) {
        fprintf(stderr, "[initHashTable] Erreur lors de l'allocation du tableau :(\n");
        return NULL;
    }

    hashTable->length = 0;
    hashTable->capacity = capacity;
    hashTable->tab = calloc(capacity, sizeof(__List *));

    if (!hashTable->tab) {
        fprintf(stderr, "[initHashTable] Erreur lors de l'allocation du tableau :(\n");
        free(hashTable);
        return NULL;
    }

    return hashTable;
}

int hashTableContains(__HashTable *hashTable, int key) {
    long long h = hash(key, hashTable->capacity);
    return listContains(hashTable->tab[h], key) != NULL;
}

Key *get(__HashTable *hashTable, int key) {
    long long h = hash(key, hashTable->capacity);
    return listContains(hashTable->tab[h], key);
}

void set(__HashTable *hashTable, int key, Key *new) {
    long long h = hash(key, hashTable->capacity);
    __List *l = hashTable->tab[h];
    while (l) {
        if (l->key == key) {
            freeKey(l->value);
            l->value = new != NULL ? copyKey(new) : NULL;
            break;
        }
        l = l->tail;
    }
}

void add(__HashTable *hashTable, int key, Key *value) {
    long long h = hash(key, hashTable->capacity);

    __List *newCell = malloc(sizeof(__List));
    if (!newCell) {
        fprintf(stderr, "[add] Erreur lors de l'allocation de newCell :(\n");
        return;
    }

    newCell->value = value != NULL ? copyKey(value) : NULL;
    newCell->key = key;
    newCell->tail = hashTable->tab[h];
    hashTable->tab[h] = newCell;
}

void freeHashTableListCell(__List *list) {
    if (!list) return;
    freeKey(list->value);
    free(list);
}

void freeHashTable(__HashTable *hashTable) {
    if (!hashTable) return;

    for (int i = 0; i < hashTable->capacity; ++i) {
        __List *l = hashTable->tab[i];
        while (l) {
            __List *tail = l->tail;
            freeHashTableListCell(l);
            l = tail;
        }
    }

    free(hashTable->tab);
    free(hashTable);
}