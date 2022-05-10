#ifndef PROJETSTR_HASH_H
#define PROJETSTR_HASH_H

#include <stdio.h>

unsigned char* duplicateHash(const unsigned char* hash);

int hashEqual(const unsigned char* a, const unsigned char* b);

void DEBUG_printHashHex(const unsigned char* hash);

char* hashToStr(const unsigned char* hash);

void writeHash(const unsigned char* hash, FILE* file);

unsigned char* readHash(const char* hash);

#endif //PROJETSTR_HASH_H
