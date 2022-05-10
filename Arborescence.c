#include "Arborescence.h"
#include "Hash.h"

// QUESTION 8.1
/**
 * @brief Alloue la mémoire pour une instance de cellTree et l'initialise avec le bloc passé en paramètre.
 * Le bloc passé en paramètre est copié.
 * @param b
 * @return
 */
CellTree* createNode(Block* b) {
    CellTree* cellTree = malloc(sizeof(CellTree));

    if (!cellTree) {
        fprintf(stderr, "[createNode] Erreur lors de l'allocation de la mémoire :(");
        return NULL;
    }

    cellTree->block = b;

    cellTree->firstChild = NULL;
    cellTree->height = 0;
    cellTree->nextBro = NULL;
    cellTree->parent = NULL;

    return cellTree;
}

// QUESTION 8.2
/**
 * @brief Met à jour la hauteur de parent après éventuelle augmentation de la hauteur de child.
 * @param parent
 * @param child
 * @return
 */
int updateHeight(CellTree* parent, CellTree* child) {
    if (child->height + 1 > parent->height){
        parent->height = child->height + 1;
        return TRUE;
    }
    return FALSE;
}

// QUESTION 8.3
/**
 * Ajoute child comme premier fils du parent et met à jour la hauteur des nœuds ascendants.
 * @param parent
 * @param child
 */
void addChild(CellTree* parent, CellTree* child) {
    child->parent = parent;
    child->nextBro = parent->firstChild;
    parent->firstChild = child;

    CellTree* c = child;
    while (c->parent) {
        // Si la hauteur d'un parent n'a pas changé alors la hauteur de ses ascendants restera inchangée également.
        if (!updateHeight(c->parent, c)) {
            break;
        }
        c = c->parent;
    }
}

// QUESTION 8.4
void printTree(CellTree* cellTree) {
    if (!cellTree) return;
    printf("< Hauteur : %d ; hash du bloc : ", cellTree->height);
    writeHash(cellTree->block->hash, stdout);
    printf(">\n");

    printTree(cellTree->nextBro);
    printTree(cellTree->firstChild);
}

// QUESTION 8.5
/**
 * @brief Supprime la mémoire associée à un nœud, i.e. le nœud lui-même et le bloc auquel il fait référence.
 * Ne supprime pas les nœuds adjacents
 * @param node
 */
void deleteNode(CellTree* node) {
    freeBlockShallow(node->block);
    free(node);
}

// QUESTION 8.5 bis
void deleteCellTree(CellTree* cellTree) {
    if (cellTree == NULL) return;
    deleteCellTree(cellTree->nextBro);
    deleteCellTree(cellTree->firstChild);

    deleteNode(cellTree);
}

// QUESTION 8.6
/**
 * @brief Calcule le fils de cellTree avec la hauteur maximale.
 * @param cellTree
 * @return
 */
CellTree* highestChild(const CellTree* cellTree) {
    int max = -1;
    CellTree* argMax = NULL;
    CellTree* child = cellTree->firstChild;

    while (child) {
        if (child->height > max) {
            max = child->height;
            argMax = child;
        }
        child = child->nextBro;
    }

    return argMax;
}

CellTree* DEPRECATED_highestChild(CellTree* cellTree) {
    int max = -1;
    CellTree* high;

    if(cellTree -> firstChild) {
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

// QUESTION 8.7
/**
 * @brief Renvoie la valeur hachée de descendant le plus lointain de cellTree.
 * @param cellTree
 * @return
 */
unsigned char* lastNode(const CellTree* cellTree) {
    const CellTree* res = cellTree;
    while(res->firstChild){
        res = highestChild(res);
    }

    return res->block->hash;
}

// QUESTION 8.8
CellProtected* buildLongestDeclarationChain(CellTree* cellTree) {
    CellProtected *res = cellTree->block->votes;
    cellTree = highestChild(cellTree);
    while (cellTree) {
        res = merge(res, cellTree->block->votes);
        cellTree = highestChild(cellTree);
    }
    return res;
}

void addChildOnLongestChain(CellTree* cellTree, CellTree* node) {
    if (!cellTree) {
        return;
    }

    while (cellTree->firstChild) {
        cellTree = highestChild(cellTree);
    }

    addChild(cellTree, node);
}
