#include "blockchain.h"
#include "rsa.h"
#include "Protected.h"
#include <stdio.h>
#include <openssl/sha.h>

void DEBUG_printHashHex(const unsigned char* hash) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        printf("%x", hash[i]);
    }

    printf("\n");
}

void writeHash(const unsigned char* hash, FILE* file) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        fprintf(file, "%x ", hash[i]);
    }
}

unsigned char* readHash(const char* hash) {
    unsigned char* res = malloc(SHA256_DIGEST_LENGTH * sizeof(unsigned char));
    if (!res) {
        fprintf(stderr, "[readHash] Erreur lors de l'allocation de la mémoire");
        return NULL;
    }

    int resIdx = 0;

    char buffer[8];
    int bufferIdx = 0;
    while (*hash != '\0') {
        if (*hash != ' ') {
            buffer[bufferIdx++] = *hash;
        } else if (bufferIdx > 0) {
            buffer[bufferIdx] = '\0';
            res[resIdx++] = (int)strtol(buffer, NULL, 16);
            printf("\t%d\n", res[resIdx - 1]);
            bufferIdx = 0;
        }

        ++hash;
    }

    //printf("hash read: ");
    //DEBUG_printHashHex(res);
    //printf("\n");
    return res;
}

/**
 * @brief Libère le bloc, son hash, le hash précédent et la structure de la liste de déclarations,
 * i.e. sans libérer les déclarations.
 * @param block Le bloc à libérer.
 */
void freeBlock(Block* block) {
    if (!block) return;

    delete_list_protected_shallow(block->votes);
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
    printf("============ DEBUG FREE BLOCK (SHALLOW) %p ============\n", block);
    if (!block) return;
    free(block->hash);
    printf("\thash freed\n");
    free(block->previous_hash);
    printf("\tprevious hash freed\n");
    free(block);
    printf("\tblock freed\n");
}

/**
 * @brief Écrit un fichier dans un bloc.
 * @param filename Nom du fichier dans lequel écrire. Sera créé si inexistant, écrasé si existant.
 * @param block Le bloc à écrire.
 */
void writeBlock(char* filename, Block* block) {
    FILE* file = fopen(filename, "w");
    unsigned char* block_str = blockToStr(block);
    printf("DEBUG writeBlock block.hash = ");
    DEBUG_printHashHex(block->hash);
    printf("\n");
    writeHash(block->hash, file);
    fprintf(file,"/%s\n", block_str);
    free(block_str);
    fclose(file);
}

/**
 * @brief Initialise un bloc avec les champs d'un bloc enregistré dans un fichier donné.
 * @param filename Nom du fichier dans lequel est enregistré le bloc à lire.
 * @return NULL en cas d'erreur, un nouveau bloc (alloué dynamiquement) sinon.
 */
Block* readBlock(char* filename) {
    char str[5000];
    FILE* file = fopen(filename, "r");
    if (fgets(str, 17999, file) == NULL) {
        fprintf(stderr, "[readBlock] Error while reading file :(\n");
        return NULL;
    }
    Block* block = strToBlock(str);
    return block;
}

unsigned char* blockToStr(Block* block) {
    // todo tester...
    unsigned char* repr = malloc(5000 * sizeof(unsigned char));
    if (!repr) {
        fprintf(stderr, "[blockToStr] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    char* authorKeyRepr = key_to_str(block->author);
    sprintf((char*) repr, "%s/%s/%d", authorKeyRepr, block->previous_hash, block->nonce);
    free(authorKeyRepr);

    CellProtected* current = block->votes;

    while (current) {
        char* protectedStr = protected_to_str(current->data);
        sprintf((char*) repr, "%s/%s", repr, protectedStr);
        current = current->next;
        free(protectedStr);
    }

    repr = realloc(repr, (strlen((char*) repr) + 1) * sizeof(char));

    return repr;
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
            }
            else if (bufferNo == 2) {
                printf("DEBUG authorKey: %s\n", buffer);
                authorKey = str_to_key(buffer);
                printf("DEBUG %lx, %lx\n", authorKey->val, authorKey->n);
            } else if (bufferNo == 3) {
                if (strcmp(buffer, "(null)") == 0) {
                    previousHash = NULL;
                } else {
                    previousHash = readHash(buffer);
                }
            } else if (bufferNo == 4) {
                nonce = atoi(buffer);
            } else {
                Protected* declaration = str_to_protected(buffer);
                votes = prependProtected(declaration, votes);
            }
        } else {
            buffer[bufferIdx++] = str[i];
        }
    }

    // S'il reste des choses dans le buffer, on le traite.
    if (bufferIdx > 0) {
        Protected* declaration = str_to_protected(buffer);
        votes = prependProtected(declaration, votes);
    }

    Block* block = malloc(sizeof(Block));
    if (!block) {
        fprintf(stderr, "[strToBlock] Erreur lors de l'allocation :(");
        freeKey(authorKey);
        free(previousHash);
        return NULL;
    }

    printf("DEBUG from str to block : hash: ");
    DEBUG_printHashHex(currentHash);
    printf("\n");
    block->author = authorKey;
    block->votes = reverseCellProteted(votes);
    block->nonce = nonce;
    block->previous_hash = previousHash;
    block->hash = currentHash;

    return block;
}

int blocksEqual(const Block* b1, const Block* b2) {
    if (b1 == b2) return TRUE;
    //printf("DEBUG different pointers\n");
    if (!b1 || !b2) return FALSE;
    //printf("DEBUG none is NULL\n");
    //printf("DEBUG b1.key = (%lx, %lx), b2.key = (%lx, %lx)\n", b1->author->val, b1->author->n, b2->author->val, b2->author->n);
    //printf("DEBUG key equal ? %s\n", keysEqual(b1->author, b2->author) ? "TRUE" : "FALSE");
    //printf("DEBUG b1.nonce = %d ; b2.nonce = %d\n", b1->nonce, b2->nonce);
    //printf("DEBUG b1.prevh = %p ; b2.prevh = %p\n", b1->previous_hash, b2->previous_hash);

    return b1->nonce == b2->nonce
        && keysEqual(b1->author, b2->author)
        && (b1->previous_hash == b2->previous_hash
            || strcmp((const char*) b1->previous_hash, (const char*) b2->previous_hash) == 0)
        && cellProtectedEqual(b1->votes, b2->votes);
}

unsigned char* strToHash(const unsigned char* str){
    return SHA256(str, strlen((char*) str),0);
}

int startsWithDZeros(const unsigned char* string, int d) {
    while (d > 0) {
        --d;
        if (string[d] != 0) {
            return FALSE;
        }
    }

    //printf("DEBUG end of startsWithDZeros : string = ");
    //DEBUG_printHashHex(string);

    return TRUE;
}

void compute_proof_of_work(Block* b, int d) {
    b->nonce = 0;
    while (TRUE) {
        unsigned char* str = blockToStr(b);
        unsigned char* hash = calloc(SHA256_DIGEST_LENGTH, sizeof(unsigned char));

        if (!hash) {
            fprintf(stderr, "[compute_proof_of_work] Erreur lors de l'allocation de la mémoire :(\n");
            free(str);
            continue;
        }

        if (!str) {
            free(hash);
            continue;
        }

        SHA256(str, strlen((const char*) str), hash);

        //printf("DEBUG hash = ");
        //DEBUG_printHashHex(hash);

        if (startsWithDZeros(hash, d)) {
            //printf("\tDEBUG hash = ");
            //DEBUG_printHashHex(hash);
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
    block->hash = (unsigned char*) strdup((char*) orig->hash);
    block->previous_hash = (unsigned char*) strdup((char*) orig->previous_hash);
    block->votes = orig->votes;
    block->author = orig->author;

    return block;
}

int verify_block(Block* B, int d) {
    //printf("DEBUG verify_block hash = ");
    //DEBUG_printHashHex(B->hash);
    //printf("\n");
    unsigned char* str = blockToStr(B);
    unsigned char* hash = malloc((SHA256_DIGEST_LENGTH + 1) * sizeof(char));
    if (hash == NULL) {
        fprintf(stderr, "[verify_block] Erreur lors de l'allocation de la mémoire :(\n");
    }

    SHA256(str, strlen((const char*) str), hash);
    int s = startsWithDZeros(hash, d);
    free(str);
    free(hash);
    return s;
}
