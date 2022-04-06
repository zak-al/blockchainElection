#include <math.h>
#include "hashsetKeyToInt.h"
#include "voting.h"

size_t find_position(HashTableKeyToInt* t, Key* key) {
    size_t idx = hash_function(key, t->size);
    while (t->tab[idx] != NULL && !keysEqual(key, t->tab[idx]->key)) {
        idx = (idx + 1) mod t->size;
    }
    return idx;
}
