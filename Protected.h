#ifndef PROJETSTR_PROTECTED_H
#define PROJETSTR_PROTECTED_H

#include "rsa.h"
#include "Signature.h"

typedef struct {
    Key *votersPublicKey;
    char *message;
    Signature *signature;
} Protected;

Protected *copyProtected(Protected *protected);

int protectedEqual(Protected *p1, Protected *p2);

Protected *init_protected(const Key* votersPublicKey, char *mess, Signature *sgn);

void freeProtected(Protected *

protected);

char *protected_to_str(Protected *p);

Protected *str_to_protected(char *str);

int verify(Protected *pr);

#endif //PROJETSTR_PROTECTED_H
