#include "blockchain.h"
#include "rsa.h"
#include "Protected.h"
#include <stdio.h>
#include <openssl/sha.h>

void freeBlock(Block* block) {
    if (!block) return;

    // freeKey(block->author);
    // todo remplacer par supression semi-profonde pour ne pas supprimer les protected derrière
    delete_list_protected(block->votes);
    free(block->hash);
    free(block->previous_hash);
    free(block);
}

void writeBlock(char* filename, Block* block){
    FILE* file = fopen(filename, "w");
    char* block_str = blockToStr(block);
    fprintf(file,"%s/%s\n", block->hash, block_str);
    free(block_str);
    fclose(file);
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

unsigned char* strToHash(const char* str){
    unsigned char* d = SHA256(str, strlen(str),0);
    int i;
    for(i=0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x",d[i]);
    }
    return d;
}

int startsWithDZeros(const char* string, int d) {
    while (d > 0) {
        --d;
        if (string[d] != '0') {
            return FALSE;
        }
    }
    return TRUE;
}

void compute_proof_of_work(Block* b, int d) {
    int bd = 4 * d;

    b->nonce = 0;
    while (TRUE) {
        ++(b->nonce);
        unsigned char* str = blockToStr(b);
        unsigned char* hash = SHA256(str, strlen(str), 0);
        if (startsWithDZeros(hash, bd)) {
            free(str);
            free(hash);
            return;
        }

        free(str);
        free(hash);
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
    char* res = blockToStr(B);
    unsigned char* hash = SHA256(res,strlen(res),0);
    for(int i = 0; i < 4*d; i++){
        if(hash[i] != 0){
            return FALSE;
        }
    }
    free(res);
    free(hash);
    return TRUE;
}
