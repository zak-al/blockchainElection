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
        fprintf(file, "%d ", hash[i]);
    }
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
    writeHash(block->hash, file);
    fprintf(file,"/%s\n", block_str);
    free(block_str);
    fclose(file);
}

Block* readBlock(char* filename, Block* block) {

}

unsigned char* blockToStr(Block* block) {
    // todo tester...
    // todo tester malloc
    unsigned char* repr = malloc(16384 * sizeof(unsigned char));
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
                currentHash = (unsigned char*) strdup(buffer);
            }
            else if (bufferNo == 2) {
                authorKey = str_to_key(buffer);
            } else if (bufferNo == 3) {
                previousHash = (unsigned char*) strdup(buffer);
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

    Block* block = malloc(sizeof(Block));
    if (!block) {
        fprintf(stderr, "[strToBlock] Erreur lors de l'allocation :(");
        freeKey(authorKey);
        free(previousHash);
        return NULL;
    }

    block->author = authorKey;
    block->votes = votes;
    block->nonce = nonce;
    block->previous_hash = previousHash;
    block->hash = currentHash;

    freeKey(authorKey);
    free(previousHash);

    return block;
}

unsigned char* strToHash(const unsigned char* str){
    return SHA256(str, strlen((char*) str),0);
}

int startsWithDZeros(const unsigned char* string, int d) {
    printf("DEBUG start of startsWithDZeros : string = ");
    DEBUG_printHashHex(string);

    while (d > 0) {
        --d;
        if (string[d] != 0) {
            return FALSE;
        }
    }

    printf("DEBUG end of startsWithDZeros : string = ");
    DEBUG_printHashHex(string);

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

        printf("DEBUG hash = ");
        DEBUG_printHashHex(hash);

        if (startsWithDZeros(hash, d)) {
            printf("\tDEBUG hash = ");
            DEBUG_printHashHex(hash);
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

int verify_block(Block* B, int d){
    unsigned char* str = blockToStr(B);
    unsigned char* hash = SHA256(str, strlen((const char*) str), 0);
    int s = startsWithDZeros( hash, 4 * d);
    free(str);
    free(hash);
    return s;
}
