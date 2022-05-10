#ifndef PROJETSTR_VOTING_H
#define PROJETSTR_VOTING_H

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"
#include "DEPRECATED_hashset.h"
#include "Signature.h"
#include "Protected.h"

typedef struct cellKey {
    Key* data;
    struct cellKey* next;
} CellKey;

typedef struct cellProtected {
    Protected* data;
    struct cellProtected* next;
} CellProtected;

void generateRandomData(int nv, int nc);

CellProtected* readProtected(char* filename);

CellKey* readPublicKeys(char* filename);

/*
 * =========== EXERCICE 5 : LISTES DE CLÉS ET DE DÉCLARATIONS ===========
 */

CellKey* createCellKey(Key* key);

CellKey* prependKey(Key* key, CellKey* list);

void printListProtected(CellProtected* list);

void deleteCellProtected(CellProtected* cellProtected);

void deleteCellProtectedShallow(CellProtected* cellProtected);

void deleteListProtected(CellProtected* cellProtected);

void deleteListProtectedShallow(CellProtected* cellProtected);

CellProtected* copyCellProtectedShallow(const CellProtected* cellProtected);

void printListKeys(CellKey* list);

CellProtected* createCellProtected(Protected* pr);

CellProtected* prependProtected(Protected* protected, CellProtected* list);

void deleteCellKey(CellKey* cellKey);

void deleteListKeys(CellKey* cellKey);

int cellProtectedEqual(const CellProtected* one, const CellProtected* two);

CellProtected* reverseCellProteted(CellProtected* list);

CellProtected* merge(CellProtected* l1, CellProtected* l2);

/*
 * =========== EXERCICE 6 ===========
 */

CellProtected* deleteListeFraude(CellProtected* cellProtected);

Key* computeWinner(CellProtected* declarations, CellKey* candidates, CellKey* voters, size_t sizeC, size_t sizeV);

#endif //PROJETSTR_VOTING_H
