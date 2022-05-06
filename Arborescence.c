#include "Arborescence.h"

CellTree* createNode(Block* b) {
    CellTree* cellTree = malloc(sizeof(CellTree));
    cellTree->block = copyBlock(b);
    cellTree->firstChild = NULL;
    cellTree->height = 0;
    cellTree->nextBro = NULL;
    cellTree->parent = NULL;

    return cellTree;
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

void printTree(CellTree* cellTree) {
    printf("La hauteur est %d, la veleur hachée est %s", cellTree->height, cellTree->block->hash);

    if(cellTree->nextBro){
        printTree(cellTree->nextBro);
    }
    if(cellTree->firstChild){
        printTree(cellTree->firstChild);
    }
}

void deleteNode(CellTree* node){
    freeBlock(node->block);
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

CellTree* highestChild(CellTree* cellTree) {
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
    CellProtected *res = cellTree->block->votes;
    cellTree = highestChild(cellTree);
    while (cellTree){
        res = fusionner(res, cellTree->block->votes);
        cellTree = highestChild(cellTree);
    }
    return res;
}

void submit_vote(Protected* p) {
    FILE* fic = fopen("Pending_votes.txt", "w+");
    if (fic == NULL){
        fprintf(stderr, "Erreur lors de l'ouverture du fichier.\n");
    }

    char* str = protected_to_str(p);

    fprintf(fic, "%s", str);

    free(str);
    fclose(fic);
}

void create_block(CellTree* cellTree, Key* author, int d){
    Block* b = malloc(sizeof(Block));

    Key* a = malloc(sizeof(Key));
    init_key(a, author -> val, author -> n);
    b -> author = a;

    CellProtected* cellProtected = read_protected("Pending_votes.txt");
    b -> votes = cellProtected;

    remove("Pending_votes.txt");

    b -> previous_hash = (unsigned char *) strdup((const char *) cellTree->block->hash);

    b -> nonce = 0;
    //compute_proof_of_work(b, d);

    unsigned char* str = blockToStr(b);
    unsigned char* hash = strToHash(str);
    b -> hash = (unsigned char *)strdup((char*) hash);

    free(str);
    free(hash);

    writeBlock("Pending_block.txt",b);

    delete_list_protected(cellProtected);
    freeBlock(b);
}

void add_block(int d, char* name){
    FILE* fic = fopen("Pending_block.txt", "r+");

    char* str = NULL;

    // todo
    fgets(str, 18000, fic);
    Block* b = strToBlock(str);

    if(verify_block(b, d)){
        FILE* f = fopen(name, "w");

        char* key = key_to_str(b -> author);

        fprintf(f, "%s\n", key);
        fprintf(f, "%s\n", b->hash);
        fprintf(f, "%s\n", b -> previous_hash);
        fprintf(f, "%d\n", b -> nonce);

        CellProtected* current = b -> votes;

        while(current != NULL){
            char* pr = protected_to_str(current -> data);

            fprintf(f, "%s", pr);
            current = current -> next;

            free(pr);
        }

        free(key);
        fclose(f);
    }


    delete_list_protected(b -> votes);
    freeBlock(b);

    remove("Pending_block.txt");
}