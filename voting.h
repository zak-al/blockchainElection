//
// Created by Zakarie Aloui on 09/03/2022.
//

#ifndef PROJETSTR_VOTING_H
#define PROJETSTR_VOTING_H

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"

typedef struct {
    long* content;
    size_t length;
} Signature;

typedef struct {
    Key* votersPublicKey;
    char* message;
    Signature* signature;
} Protected;

Signature* init_signature(long* content, size_t size);
Signature* sign(char* mess, Key* sKey);
char* signature_to_str(Signature* sgn);
Signature* str_to_siganture(char* str);
Protected* init_protected(Key* pKey, char* mess, Signature* sgn);
int verify(Protected* pr);

void freeSignature(Signature* signature);
void freeProtected(Protected* protected);

#endif //PROJETSTR_VOTING_H
