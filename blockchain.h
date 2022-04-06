//
// Created by Zhichun Hua on 06/04/2022.
//

#ifndef BLOCKCHAINELECTION_BLOCKCHAIN_H
#define BLOCKCHAINELECTION_BLOCKCHAIN_H

#include "rsa.h"
#include "voting.h"

typedef struct block{
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
}Block;

char* blockToStr(Block* block);
void write_fichier(char* filename, Block* block);
Block* strToBlock(char* str);

#endif //BLOCKCHAINELECTION_BLOCKCHAIN_H
