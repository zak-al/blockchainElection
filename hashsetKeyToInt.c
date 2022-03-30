#include "hashsetKeyToInt.h"

HashCell* create_hashcell(Key* key) {
    HashCell *hashCell = malloc(sizeof(HashCell));
    if (!newCell) {
        fprintf(stderr, "[create_hashcell] Erreur lors de l'allocation :(\n");
        return NULL;
    }

    hashCell->key = copyKey(key);
    hashCell->val = 0;
}

int hash_function(Key* key, int size) {
    double A = (sqrt(5) - 1) / 2;
    long x = key->n + key->val
    return floor(size * (x * A - floor(x * A)));
}

int find_position(HashTable* t, Key* key) {
    size_t idx = hash_function(key, t->size);
    while (t->tab[idx] == NULL || t->tab[idx]) {

    }
}

HashTableKeyToInt* create_hashtable(CellKey* keys, int size){
    HashTableKeyToInt* newtable = malloc(sizeof(HashTableKeyToInt));
    if (!hashtable) {
        fprintf(stderr, "[create_hashcell] Erreur lors de l'allocation :(\n");
        return NULL;
    }
    hashtable->size = size;

}
