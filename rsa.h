//
// Created by Zakarie Aloui on 01/03/2022.
//

#ifndef PROJETSTR_RSA_H
#define PROJETSTR_RSA_H

#include <stdlib.h>
#include <string.h>
#include "primalite.h"

long extended_gcd(long s, long t, long* u, long* v);
void generate_key_values(long p, long q, long* n, long* s, long* u);

long* encrypt(const char* string, long s, long n);
char* decrypt(const long* string, size_t size, long u, long n);

#endif //PROJETSTR_RSA_H
