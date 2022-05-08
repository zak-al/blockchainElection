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

// Question 7.1
void writeBlock(char* filename, Block* block);
Block* readBlock(char* filename);
int blocksEqual(const Block* b1, const Block* b2);

void DEBUG_printHashHex(const unsigned char* hash);

void writeHash(const unsigned char* hash, FILE* file);
unsigned char* readHash(const char* hash);

void freeBlock(Block* block);
void freeBlockShallow(Block* block);
Block* copyBlock(Block* orig);

unsigned char* blockToStr(Block* block);
Block* strToBlock(char* str);
unsigned char* strToHash (const unsigned char* str);

int startsWithDZeros(const unsigned char* string, int d);
void compute_proof_of_work(Block* B, int d);
int verify_block(Block* B, int d);

#endif //BLOCKCHAINELECTION_BLOCKCHAIN_H
