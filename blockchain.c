#include "blockchain.h"
#include "rsa.h"
#include "Protected.h"
#include <stdio.h>

void freeBlock(Block* block) {
    if (!block) return;

    freeKey(block->author);
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

char* blockToStr(Block* block) {
    // todo tester...
    char* repr = malloc(16384 * sizeof(char));
    char* authorKeyRepr = key_to_str(block->author);
    sprintf(repr, "%s/%s/%d", authorKeyRepr, block->previous_hash, block->nonce);
    free(authorKeyRepr);

    CellProtected* current = block->votes;

    while (current) {
        char* protectedStr = protected_to_str(current->data);
        sprintf(repr, "%s/%s", repr, protectedStr);
        current = current->next;
        free(protectedStr);
    }

    repr = realloc(repr, (strlen(repr) + 1) * sizeof(char));

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

void compute_proof_of_work(Block* B, int d){
    int valide = 0;

    while(valide == 0){
        valide = 1;
        char* res = blockToStr(B);
        char* hash = SHA256(res,strlen(res),0);
        for(int i = 0; i < 4*d; i++){
            if(hash[i] != 0){
                valide = 0;
                free(res);
                free(hash);
                continue;
            }
        }
        B->nonce = B->nonce + 1;
        free(hash);
        free(res);
    }

    B->nonce =B->nonce - 1;
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
