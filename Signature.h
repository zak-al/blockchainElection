#ifndef PROJETSTR_SIGNATURE_H
#define PROJETSTR_SIGNATURE_H

#include <stdlib.h>
#include "rsa.h"

typedef struct {
    long *content;
    size_t length;
} Signature;

int signaturesEqual(Signature *s1, Signature *s2);

void printLongVector(long *result, size_t size);

long *copyLongArray(const long *array, size_t size);

Signature *copySignature(Signature *signature);

Signature *initSignature(const long *content, size_t size);

Signature *sign(const char *mess, const Key *sKey);

char *signature_to_str(Signature *sgn);

Signature *str_to_signature(char *str);

void freeSignature(Signature *signature);

#endif //PROJETSTR_SIGNATURE_H
