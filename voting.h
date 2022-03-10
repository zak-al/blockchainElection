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

Signature* init_signature(long* content, size_t size);

Signature* sign(char* mess, Key* sKey);

#endif //PROJETSTR_VOTING_H
