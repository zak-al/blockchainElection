//
// Created by Zakarie Aloui on 09/03/2022.
//

#ifndef PROJETSTR_VOTING_H
#define PROJETSTR_VOTING_H

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"
#include "hashset.h"

/*
 * =========== DÉFINITION ET MÉTHODES DE SIGNATURE ===========
 */
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


/*
 * =========== DÉFINITION ET MÉTHODES DE PROTECTED ===========
 */

typedef struct {
    Key *votersPublicKey;
    char *message;
    Signature *signature;
} Protected;

int protectedEqual(Protected *p1, Protected *p2);

Protected *init_protected(Key *votersPublicKey, char *mess, Signature *sgn);

void freeProtected(Protected *

protected);

char *protected_to_str(Protected *p);

Protected *str_to_protected(char *str);

int verify(Protected *pr);

/*
 * =========== EXERCICE 4 : GÉNÉRATION DE DONNÉES POUR L'ÉLECTION + LECTURE DES CLÉS ET DÉCLARATIONS ===========
 */

typedef struct cellKey {
    Key *data;
    struct cellKey *next;
} CellKey;

typedef struct cellProtected {
    Protected *data;
    struct cellProtected *next;
} CellProtected;

void generate_random_data(int nv, int nc);

CellProtected *read_protected();

CellKey *read_public_keys(char *filename);

/*
 * =========== EXERCICE 5 : LISTES DE CLÉS ET DE DÉCLARATIONS ===========
 */

CellKey *create_cell_key(Key *key);

CellKey *prependKey(Key *key, CellKey *list);

void print_list_protected(CellProtected *list);

void delete_cell_protected(CellProtected *cellProtected);

void delete_list_protected(CellProtected *cellProtected);

void print_list_keys(CellKey *list);

CellProtected *create_cell_protected(Protected *pr);

CellProtected *prependProtected(Protected *

protected,
CellProtected *list
);

void delete_cell_key(CellKey *cellKey);

void delete_list_keys(CellKey *cellKey);

/*
 * =========== EXERCICE 6 ===========
 */

void delete_liste_fraude(CellProtected *cellProtected);

void delete_cell_protected(CellProtected** cellProtected);
void delete_list_protected(CellProtected** cellProtected);
void delete_liste_fraude(CellProtected** cellProtected);

#endif //PROJETSTR_VOTING_H
