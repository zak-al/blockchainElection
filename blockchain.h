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

// Question 7.1
void writeBlock(char* filename, Block* block);

Block* readBlock(char* filename);

int blocksEqual(const Block* b1, const Block* b2);

void freeBlock(Block* block);

void freeBlockDeep(Block* block);

void freeBlockShallow(Block* block);

unsigned char* blockToStr(Block* block);

Block* strToBlock(char* str);

unsigned char* hashString(const unsigned char* str);

int startsWithDZeros(const unsigned char* string, int d);

void computeProofOfWork(Block* b, int d);

int verify_block(Block* B, int d);

#endif //BLOCKCHAINELECTION_BLOCKCHAIN_H
