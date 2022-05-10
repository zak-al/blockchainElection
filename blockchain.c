#include "blockchain.h"
#include "rsa.h"
#include "Protected.h"
#include "Hash.h"
#include <stdio.h>
#include <openssl/sha.h>

/**
 * @brief Libère le bloc, son hash, le hash précédent et le liste de déclarations.
 * @param block Le bloc à libérer.
 */
void freeBlockDeep(Block* block) {
    if (!block) return;

    deleteListProtected(block->votes);
    free(block->hash);
    free(block->previous_hash);
    free(block);
}

/**
 * @brief Libère le bloc, son hash, le hash précédent et la structure de la liste de déclarations,
 * i.e. sans libérer les déclarations. Ne libère pas la clé de l'auteur.
 * @param block Le bloc à libérer.
 */
void freeBlock(Block* block) {
    if (!block) return;

    deleteListProtectedShallow(block->votes);
    free(block->hash);
    free(block->previous_hash);
    free(block);
}

/**
 * @brief Libère le bloc, son hash et le hash précédent. En particulier, ne libère pas la liste des déclarations
 * (ni la structure, ni le contenu).
 * @param block Le bloc à libérer.
 */
void freeBlockShallow(Block* block) {
    if (!block) return;
    free(block->hash);
    free(block->previous_hash);
    free(block);
}

/**
 * @brief Écrit un fichier dans un bloc.
 * @param filename Nom du fichier dans lequel écrire. Sera créé si inexistant, écrasé si existant.
 * @param block Le bloc à écrire.
 */
void writeBlock(char* filename, Block* block) {
    FILE* file = fopen(filename, "w");
    char* authorKeyRepr = keyToStr(block->author);
    unsigned char* blockRepr = blockToStr(block);

    writeHash(block->hash, file);
    fprintf(file, "/%s\n", blockRepr);

    free(authorKeyRepr);
    free(blockRepr);
    fclose(file);
}

/**
 * @brief Initialise un bloc avec les champs d'un bloc enregistré dans un fichier donné.
 * @param filename Nom du fichier dans lequel est enregistré le bloc à lire.
 * @return NULL en cas d'erreur, un nouveau bloc (alloué dynamiquement) sinon.
 */
Block* readBlock(char* filename) {
    char str[2500];
    FILE* file = fopen(filename, "r");
    if (fgets(str, 2499, file) == NULL) {
        fprintf(stderr, "[readBlock] Error while reading file :(\n");
        return NULL;
    }
    Block* block = strToBlock(str);
    return block;
}

unsigned char* blockToStr(Block* block) {
    unsigned char* repr = calloc(2500, sizeof(unsigned char));
    if (!repr) {
        fprintf(stderr, "[blockToStr] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    char* authorKeyRepr = keyToStr(block->author);
    char* prevHStr = hashToStr(block->previous_hash);

    sprintf((char*) repr, "%s/%s/%d", authorKeyRepr, prevHStr, block->nonce);
    free(authorKeyRepr);
    free(prevHStr);
    CellProtected* current = block->votes;
    while (current) {
        char* protectedStr = protectedToStr(current->data);
        sprintf((char*) repr, "%s/%s", repr, protectedStr);
        current = current->next;
        free(protectedStr);
    }
    return realloc(repr, (strlen((char*) repr) + 1) * sizeof(char));
}

Block* strToBlock(char* str) {
    Key* authorKey;
    unsigned char* previousHash;
    unsigned char* currentHash;
    int nonce;
    CellProtected* votes = NULL;

    char buffer[256];
    size_t len = strlen(str);
    int bufferIdx = 0;
    int bufferNo = 0;
    for (size_t i = 0; i < len; ++i) {
        if (str[i] == '/') {
            buffer[bufferIdx] = '\0';
            bufferIdx = 0;
            ++bufferNo;

            if (bufferNo == 1) {
                currentHash = readHash(buffer);
            } else if (bufferNo == 2) {
                authorKey = strToKey(buffer);
            } else if (bufferNo == 3) {
                if (strcmp(buffer, "(null)") == 0) {
                    previousHash = NULL;
                } else {
                    previousHash = readHash(buffer);
                }
            } else if (bufferNo == 4) {
                nonce = atoi(buffer);
            } else {
                Protected* declaration = strToProtected(buffer);
                votes = prependProtected(declaration, votes);
            }
        } else {
            buffer[bufferIdx++] = str[i];
        }
    }

    // S'il reste des choses dans le buffer, on le traite.
    if (bufferIdx > 0) {
        Protected* declaration = strToProtected(buffer);
        votes = prependProtected(declaration, votes);
    }

    Block* block = malloc(sizeof(Block));
    if (!block) {
        fprintf(stderr, "[strToBlock] Erreur lors de l'allocation :(");
        freeKey(authorKey);
        free(previousHash);
        return NULL;
    }

    block->author = authorKey;
    block->votes = reverseCellProteted(votes);
    block->nonce = nonce;
    block->previous_hash = previousHash;
    block->hash = currentHash;

    return block;
}

int blocksEqual(const Block* b1, const Block* b2) {
    if (b1 == b2) return TRUE;
    if (!b1 || !b2) return FALSE;

    return b1->nonce == b2->nonce
           && keysEqual(b1->author, b2->author)
           && (b1->previous_hash == b2->previous_hash
               || hashEqual(b1->previous_hash, b2->previous_hash))
           && cellProtectedEqual(b1->votes, b2->votes);
}

unsigned char* hashString(const unsigned char* str) {
    return SHA256(str, strlen((char*) str), 0);
}

int startsWithDZeros(const unsigned char* string, int d) {
    while (d > 0) {
        --d;
        if (string[d] != 0) {
            return FALSE;
        }
    }

    return TRUE;
}

void computeProofOfWork(Block* b, int d) {
    b->nonce = 0;
    while (TRUE) {
        unsigned char* str = blockToStr(b);
        unsigned char* hash = calloc(SHA256_DIGEST_LENGTH, sizeof(unsigned char));
        if (!hash) {
            fprintf(stderr, "[computeProofOfWork] Erreur lors de l'allocation de la mémoire :(\n");
            free(str);
            continue;
        }

        if (!str) {
            free(hash);
            continue;
        }

        SHA256(str, strlen((const char*) str), hash);

        if (startsWithDZeros(hash, d)) {
            b->hash = hash;
            free(str);
            return;
        }

        free(str);
        free(hash);
        ++(b->nonce);
    }
}

Block* copyBlock(Block* orig) {
    /*
     * Copie semi-profonde. Il n'y a pas de copie des élements de la chaîne votes ni de la clé de l'auteur.
     */
    Block* block = malloc(sizeof(Block));
    block->nonce = orig->nonce;
    block->hash = duplicateHash(orig->hash);
    block->previous_hash = duplicateHash(orig->previous_hash);
    block->votes = orig->votes;
    block->author = orig->author;

    return block;
}

int verify_block(Block* B, int d) {
    unsigned char* str = blockToStr(B);
    int s = startsWithDZeros(B->hash, d);
    free(str);
    return s;
}
