#ifndef PROJETSTR_RSA_H
#define PROJETSTR_RSA_H

#include <stdlib.h>
#include <string.h>
#include "primalite.h"

typedef struct {
    long val;
    long n;
} Key;

int keysEqual(const Key* x, const Key* y);

size_t max(long x, long y);

Key* copyKey(const Key *key);

long extended_gcd(long s, long t, long* u, long* v);
void generate_key_values(long p, long q, long* n, long* s, long* u);

long* encrypt(const char* string, long s, long n);
char* decrypt(const long* string, size_t size, long u, long n);

void init_key(Key* key, long val, long n);
void init_pair_keys(Key* pKey, Key* sKey, int low_size, int up_size);
char* key_to_str(Key* key);
Key* str_to_key(char* repr);

void freeKey(Key* key);

#endif //PROJETSTR_RSA_H
