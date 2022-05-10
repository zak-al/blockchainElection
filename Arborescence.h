#ifndef PROJETSTR_ARBORESCENCE_H
#define PROJETSTR_ARBORESCENCE_H

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

void addChild(CellTree* parent, CellTree* child);

void printTree(CellTree* cellTree);

void deleteNode(CellTree* node);

void deleteCellTree(CellTree* cellTree);

CellTree* highestChild(const CellTree* cellTree);

unsigned char* lastNode(const CellTree* cellTree);

CellProtected* buildLongestDeclarationChain(CellTree* cellTree);

void addChildOnLongestChain(CellTree* cellTree, CellTree* node);

#endif //PROJETSTR_ARBORESCENCE_H
