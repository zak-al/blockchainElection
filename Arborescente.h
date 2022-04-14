#ifndef PROJETSTR_ARBORESCENTE_H
#define PROJETSTR_ARBORESCENTE_H

#include "blockchain.h"

typedef struct block_tree_cell {
    Block* block;
    struct block_tree_cell* parent;
    struct block_tree_cell* firstChild;
    struct block_tree_cell* nextBro;
    int height;
} CellTree;

CellTree* createNode(Block* b);

int updateHeight(CellTree* parent, CellTree* child);
void addChild(CellTree* parent,CellTree* child);
void printTree(CellTree* cellTree);
void deleteNode(CellTree* node);
void deleteCellTree(CellTree* cellTree);
CellTree* highestChild(CellTree* cellTree);
CellTree* lastNode(CellTree* tree);
CellProtected* fusionner(CellProtected* cp1, CellProtected* cp2);
CellProtected* declarationLongest(CellTree* cellTree);

#endif //PROJETSTR_ARBORESCENTE_H
