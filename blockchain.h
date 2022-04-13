//
// Created by Zhichun Hua on 06/04/2022.
//

#ifndef BLOCKCHAINELECTION_BLOCKCHAIN_H
#define BLOCKCHAINELECTION_BLOCKCHAIN_H

#include "rsa.h"
#include "voting.h"

typedef struct {
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
} Block;

void freeBlock(Block* block);

void copyBlock();

char* blockToStr(Block* block);
void writeBlock(char* filename, Block* block);
Block* strToBlock(char* str);
unsigned char* strToHash(const char* str);

void compute_proof_of_work(Block* B, int d);
int verify_block(Block* B, int d);
#endif //BLOCKCHAINELECTION_BLOCKCHAIN_H
