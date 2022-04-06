//
// Created by Zhichun Hua on 06/04/2022.
//

#include "blockchain.h"
#include "rsa.h"
#include <stdio.h>

void write_fichier(char* filename, Block* block){
    FILE* file = fopen(filename, "w");
    char* block_str = block_to_str(block);
    fprintf(file,"%s/%s \n", block->hash, block_str);
    free(block_str);
}

