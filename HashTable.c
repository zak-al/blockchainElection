#include <stdio.h>
#include <math.h>
#include "HashTable.h"
#include "voting.h"

/*
* =========== htIk : table de hachage entier vers Key* ===========
*/

/**
* @brief Alloue dynamiquement une cellule de table de hachage avec la clé spécifiée.
* @param key
* @return
*/
HashTableCell* htIkCreateHashCell(int key) {
    HashTableCell* hashCell = malloc(sizeof(HashTableCell));
    if (!hashCell) {
        fprintf(stderr, "[htIkCreateHashCell] Erreur lors de l'allocation :(\n");
        return NULL;
    }

    hashCell->integer = key;
    hashCell->key = NULL;

    return hashCell;
}

void htIkAdd(const HashTable* table, int key, const Key* value) {
    size_t idx = htIkFindPosition(table, key);
    if (table->array[idx] == NULL) {
        table->array[idx] = htIkCreateHashCell(key);
        table->array[idx]->key = copyKey(value);
    }
}

size_t htIkFindPosition(const HashTable* t, int key) {
    //printf("DEBUG htikfindpos\n");
    size_t idx = hashInt(key, t->capacity);
    //printf("DEBUG idx = %ld\n", idx);

    // On cherche tant qu'on ne tombe ni sur une case nulle
    // ni sur une case déjà occupée par la clé désirée.
    while (t->array[idx] != NULL && t->array[idx]->integer != key) {
        idx = (idx + 1) mod t->capacity;
        //printf("DEBUG idx now: %ld\n", idx);
    }
    return idx;
}

/**
* @brief Vérifie l'existence d'une entrée associée à la clé key dans la table table.
* @param table
* @param key
* @return TRUE si une telle entrée existe, FALSE sinon.
*/
int htIkExists(HashTable* table, int key) {
    //printf("DEBUG exists?\n");
    size_t idx = htIkFindPosition(table, key);
    //printf("DEBUG idx\n");
    if (table->array[idx] == NULL) return FALSE;
    return TRUE;
}

Key* htIkGetOrNull(HashTable* table, int key) {
    size_t idx = htIkFindPosition(table, key);
    if (table->array[idx] == NULL) return NULL;
    return table->array[idx]->key;
}

HashTable* htIkConstruct(size_t capacity) {
    HashTable* hashTable = malloc(sizeof(HashTable));
    if (!hashTable) {
        fprintf(stderr, "[htIkConstruct] Erreur lors de l'allocation :(");
        return NULL;
    }

    hashTable->array = calloc(capacity, sizeof(HashTableCell));
    hashTable->capacity = capacity;

    if (!hashTable->array) {
        fprintf(stderr, "[htIkConstruct] Erreur lors de l'allocation du tableau :(");
        free(hashTable);
        return NULL;
    }

    return hashTable;
}

size_t hashInt(int i, size_t capacity) {
    return i mod capacity;
}

/*
* =========== htKi : table de hachage Key* vers entier ===========
*/

void htKiAdd(HashTable* table, Key* key, int value) {
    size_t idx = htKiFindPosition(table, key);
    if (table->array[idx] == NULL) {
        table->array[idx] = htKiCreateHashCell(key);
        table->array[idx]->integer = value;
    }
}

HashTableCell* htKiCreateHashCell(Key* key) {
    HashTableCell* hashCell = malloc(sizeof(HashTableCell));
    if (!hashCell) {
        fprintf(stderr, "[htKiCreateHashCell] Erreur lors de l'allocation :(\n");
        return NULL;
    }

    hashCell->key = copyKey(key);
    hashCell->integer = 0;

    return hashCell;
}

int hashKey(const Key* key, size_t capacity) {
    double A = (sqrt(5) - 1) / 2;
    long x = key->n + key->val;
    return floor(capacity * (x * A - floor(x * A)));
}

size_t htKiFindPosition(const HashTable* t, const Key* key) {
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
    if (table->array[idx] == NULL) return NULL;

    int* res = malloc(sizeof(int));

    if (!res) {
        fprintf(stderr, "[htKiGetOrNull] Erreur lors de l'allocation :(");
        return NULL;
    }

    *res = table->array[idx]->integer;
    return res;
}

int htKiExists(const HashTable* table, const Key* key) {
    size_t idx = htKiFindPosition(table, key);
    if (table->array[idx] == NULL) return FALSE;
    return TRUE;
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
    //printf("\tDEBUG key deleted\n");
    free(hashCell);
    //printf("\tDEBUG hash cell deleted\n");
}

void deleteHashTable(HashTable* hashTable) {
    if (!hashTable) return;

    for (int i = 0; i < hashTable->capacity; i++) {
        //printf("DEBUG deleting hash cell\n");
        deleteHashCell(hashTable->array[i]);
        //printf("\tDEBUG deleted\n");
    }
    free(hashTable->array);
    //printf("DEBUG array deleted\n");
    free(hashTable);
    //printf("DEBUG hashtable deleted\n");
}
