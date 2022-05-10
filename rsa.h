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

Key* copyKey(const Key* key);

long extendedGcd(long s, long t, long* u, long* v);

void generateKeyValues(long p, long q, long* n, long* s, long* u);

long* encrypt(const char* string, long s, long n);

char* decrypt(const long* string, size_t size, long u, long n);

void initKey(Key* key, long val, long n);

void initPairKeys(Key* pKey, Key* sKey, int low_size, int up_size);

char* keyToStr(Key* key);

Key* strToKey(char* repr);

void freeKey(Key* key);

#endif //PROJETSTR_RSA_H
