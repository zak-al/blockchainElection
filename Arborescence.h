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
void addChild(CellTree* parent,CellTree* child);
void printTree(CellTree* cellTree);
void deleteNode(CellTree* node);
void deleteCellTree(CellTree* cellTree);
CellTree* highestChild(CellTree* cellTree);
CellTree* lastNode(CellTree* tree);
CellProtected* fusionner(CellProtected* cp1, CellProtected* cp2);
CellProtected* declarationLongest(CellTree* cellTree);
void submit_vote(Protected* p);
void create_block(CellTree* tree, Key* author, int d);
void add_block(int d, char* name);

#endif //PROJETSTR_ARBORESCENCE_H
