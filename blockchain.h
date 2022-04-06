//
// Created by Zhichun Hua on 06/04/2022.
//

#ifndef BLOCKCHAINELECTION_BLOCKCHAIN_H
#define BLOCKCHAINELECTION_BLOCKCHAIN_H
typedef struct block{
    Key* author;
    CellProtected* votes;
    unsigned char* hash;
    unsigned char* previous_hash;
    int nonce;
}Block;

void write_fichier(char* filename, Block* block);


#endif //BLOCKCHAINELECTION_BLOCKCHAIN_H
