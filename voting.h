#ifndef PROJETSTR_VOTING_H
#define PROJETSTR_VOTING_H

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"
#include "hashset.h"
#include "Signature.h"
#include "Protected.h"

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

CellProtected *prependProtected(Protected *protected, CellProtected *list);

void delete_cell_key(CellKey *cellKey);

void delete_list_keys(CellKey *cellKey);

/*
 * =========== EXERCICE 6 ===========
 */

CellProtected *delete_liste_fraude(CellProtected *cellProtected);
Key* computeWinner(CellProtected* declarations, CellKey* candidates, CellKey* voters, size_t sizeC, size_t sizeV);

#endif //PROJETSTR_VOTING_H
