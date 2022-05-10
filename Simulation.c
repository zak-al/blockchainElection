#include <dirent.h>
#include "Simulation.h"
#include "Constants.h"
#include "Hash.h"

// QUESTION 9.1
void submitVote(Protected* p) {
    if (!p) {
        fprintf(stderr, "[submitVote] p est nul :(\n");
        return;
    }

    FILE* fic = fopen(PENDING_VOTES, "a");
    if (fic == NULL) {
        fprintf(stderr, "[submitVote] Erreur lors de l'ouverture du fichier :(\n");
        return;
    }

    char* str = protectedToStr(p);

    fprintf(fic, "%s\n", str);

    free(str);
    fclose(fic);
}

// QUESTION 9.2
/**
 * @brief Crée un bloc avec l'auteur passé en paramètre et les déclarations contenues dans le fichier Pending_votes.txt.
 *  Le bloc est ensuite écrit dans le fichier Pending_block.txt.
 *  Supprime Pending_votes.txt.
 * @param cellTree
 * @param author
 * @param d Paramètre de la preuve de travail : nombre de zéros requis en tête du hash en base 16.
 */
CellTree* createBlock(CellTree* cellTree, Key* author, int d) {
    CellProtected* cellProtected = readProtected(PENDING_VOTES);
    remove(PENDING_VOTES);

    Block* b = malloc(sizeof(Block));
    if (!b) {
        fprintf(stderr, "[createBlock] Erreur lors de l'allocation de la mémoire :(\n");
    }

    // Initialisation des champs du bloc.
    b->author = author;
    b->votes = cellProtected;

    // Si cellTree est nul alors on va ajouter le premier nœud, donc le champ previousHash doit être nul.
    if (cellTree == NULL) {
        b->previous_hash = NULL;
    } else {
        b->previous_hash = duplicateHash(lastNode(cellTree));
    }

    computeProofOfWork(b, d);
    writeBlock(PENDING_BLOCK, b);

    if (cellTree == NULL) {
        return createNode(b);
    }

    addChildOnLongestChain(cellTree, createNode(b));

    return cellTree;
}

// QUESTION 9.3
/**
 * @brief Ajoute le bloc décrit dans Pending_block.txt dans un nouveau fichier nommé name dans le répertoire Blockchain,
 * si le bloc est valide.
 * Supprime Pending_block.
 * @param d Paramètre de la preuve de travail.
 * @param name Nom du fichier. Ne doit pas excéder 115 caractères.
 */
void addBlock(int d, char* name) {
    FILE* fic = fopen(PENDING_BLOCK, "r+");
    char str[2500];
    fgets(str, 2499, fic);
    Block* b = strToBlock(str);

    if (verify_block(b, d)) {
        char filename[128];
        sprintf(filename, "Blockchain/%s", name);
        writeBlock(filename, b);
    }

    remove(PENDING_BLOCK);
}

int hashToIntModN(const unsigned char* hash, int N) {
    if (!hash) return 0;

    int m = 0;
    while (*hash) {
        m += *hash;
        ++hash;
    }

    return m mod N;
}

// QUESTION 9.4
CellTree* _readTree() {
    DIR* blockchainDir = opendir("./Blockchain/");
    if (!blockchainDir) {
        fprintf(stderr, "[readTree] Erreur lors de la lecture de répertoire.\n");
    }
    int numberOfFiles = 0;
    struct dirent* dir = NULL;
    while ((dir = readdir(blockchainDir))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            ++numberOfFiles;
        }
    }
    closedir(blockchainDir);

    CellTree** T = calloc(numberOfFiles, sizeof(CellTree*));
    CellTree* root = NULL;

    blockchainDir = opendir("./Blockchain/");
    while ((dir = readdir(blockchainDir))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char filename[128];
            sprintf(filename, "./Blockchain/%s", dir->d_name);

            Block* block = readBlock(filename);

            int idx = hashToIntModN(block->previous_hash, numberOfFiles);
            CellTree* newNode = createNode(block);
            newNode->nextBro = T[idx];
            T[idx] = newNode;
        }
    }
    closedir(blockchainDir);

    for (int i = 0; i < numberOfFiles; ++i) {
        CellTree* curr = T[i];
        while (curr) {
            if (curr->block->previous_hash == NULL) {
                root = curr;
            }

            int childIdx = hashToIntModN(curr->block->hash, numberOfFiles);
            CellTree* potentialChild = T[childIdx];

            while (potentialChild) {
                if (potentialChild->block->previous_hash == NULL) {
                    potentialChild = potentialChild->nextBro;
                    continue;
                }

                if (hashEqual(potentialChild->block->previous_hash, curr->block->hash)) {
                    addChild(curr, potentialChild);
                }
                potentialChild = potentialChild->nextBro;
            }
            curr = curr->nextBro;
        }
    }

    free(T);

    return root;
}

CellTree* readTree(void) {
    DIR* blockchainDir = opendir("./Blockchain/");
    if (!blockchainDir) {
        fprintf(stderr, "[readTree] Erreur lors de la lecture de répertoire.\n");
    }

    int numberOfFiles = 0;
    struct dirent* dir = NULL;
    while ((dir = readdir(blockchainDir))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            ++numberOfFiles;
        }
    }
    closedir(blockchainDir);

    CellTree** T = calloc(numberOfFiles, sizeof(CellTree*));
    CellTree* root = NULL;

    int k = 0;

    blockchainDir = opendir("./Blockchain/");
    while ((dir = readdir(blockchainDir))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char filename[128];
            sprintf(filename, "./Blockchain/%s", dir->d_name);

            Block* block = readBlock(filename);

            CellTree* newNode = createNode(block);
            newNode->nextBro = T[k];
            T[k] = newNode;
            ++k;
        }
    }
    closedir(blockchainDir);

    for (int i = 0; i < numberOfFiles; ++i) {
        CellTree* curr = T[i];
        if (curr->block->previous_hash == NULL) {
            root = curr;
        }

        for (int j = 0; j < numberOfFiles; ++j) {
            if (i == j) continue;
            CellTree* potentialChild = T[j];

            if (hashEqual(potentialChild->block->previous_hash, curr->block->hash)) {
                addChild(curr, potentialChild);
            }
        }
    }

    free(T);

    return root;
}


Key* computeWinnerBT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV) {
    CellProtected* votes = buildLongestDeclarationChain(tree);
    votes = deleteListeFraude(votes);
    return computeWinner(votes, candidates, voters, sizeC, sizeV);
}
