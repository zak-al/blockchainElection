#include "Arborescente.h"

CellTree* createNode(Block* b) {
    CellTree* cellTree = malloc(sizeof(CellTree));
    cellTree->block = copyBlock(b);
    cellTree->firstChild = NULL;
    cellTree->height = 0;
    cellTree->nextBro = NULL;
    cellTree->parent = NULL;
}

int updateHeight(CellTree* parent, CellTree* child) {
    if(child->height + 1 > parent->height){
        parent->height = child->height + 1;
        return 1;
    }
    return 0;
}

void addChild(CellTree* parent,CellTree* child){
    child->parent = parent;
    child->nextBro = parent->firstChild;
    parent->firstChild = child;
    updateHeight(parent, child);
}

void printTree(CellTree* cellTree){

    printf("La hauteur est %d, la veleur hachée est %s", cellTree->height, cellTree->block->hash);

    if(cellTree->nextBro){
        printTree(cellTree->nextBro);
    }
    if(cellTree->firstChild){
        printTree(cellTree->firstChild);
    }
}

void deleteNode(CellTree* node){
    delete_block(node->block);
    free(node);
}


void deleteCellTree(CellTree* cellTree){
    if(cellTree->nextBro){
        deleteCellTree(cellTree->nextBro);
    }
    if(cellTree->firstChild){
        deleteCellTree(cellTree->firstChild);
    }
    deleteNode(cellTree);
}
CellTree* highestChild(CellTree* cellTree){
    int max = -1;
    CellTree* high;

    if(cellTree -> firstChild){
        CellTree* t = cellTree -> firstChild;
        CellTree* bro = t;
        while(bro -> nextBro){
            if(bro -> height > max){
                max = bro -> height;
                high = bro;
            }
            bro = bro -> nextBro;
        }
        return high;
    }

    return NULL;
}

CellTree* lastNode(CellTree* cellTree){
    CellTree* res = cellTree;
    while(res->firstChild){
        res = highestChild(res);
    }
    return res;
}

CellProtected* fusionner(CellProtected* cp1, CellProtected* cp2){
    while (cp2) {
        prependProtected(cp2->data, cp1);
        cp2 = cp2->next;
    }
    return cp1;
}

CellProtected* declarationLongest(CellTree* cellTree){
    CellProtected *res;
    while (cellTree){
        res = fusionner(res, cellTree->block->votes);
        cellTree = highestChild(cellTree);
    }
    return res;
}
