//
// Created by Zhichun Hua on 06/04/2022.
//

#include "blockchain.h"
#include "rsa.h"
#include <stdio.h>

void write_fichier(char* filename, Block* block){
    FILE* file = fopen(filename, "w");
    char* key = key_to_str(block->author);

}