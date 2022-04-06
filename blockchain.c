//
// Created by Zhichun Hua on 06/04/2022.
//

#include "blockchain.h"
#include "rsa.h"
#include <stdio.h>

void write_fichier(char* filename, Block* block){
    FILE* file = fopen(filename, "w");
    char* block_str = blockToStr(block);
    fprintf(file,"%s/%s\n", block->hash, block_str);
    free(block_str);
}

char* blockToStr(Block* block) {
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
    int nonce;
    CellProtected* votes;

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
                authorKey = str_to_key(buffer);
            }
            else if (bufferNo == 2) {
                previousHash = (unsigned char*) strdup(buffer);
            }
            if (bufferNo == 3) {
                nonce = atoi(buffer);
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

    freeKey(authorKey);
    free(previousHash);
}
