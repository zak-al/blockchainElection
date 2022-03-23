//
// Created by Zakarie Aloui on 09/03/2022.
//

#ifndef PROJETSTR_VOTING_H
#define PROJETSTR_VOTING_H

#include <stdlib.h>
#include <stdio.h>
#include "rsa.h"

/**
 * A signature is characterised by an array of integers and its length.
 * 
 */
typedef struct {
    long* content;
    size_t length;
} Signature;

typedef struct {
    Key* votersPublicKey;
    char* message;
    Signature* signature;
} Protected;

typedef struct cellKey {
    Key* data;
    struct cellKey* next;
} CellKey;

typedef struct cellProtected {
    Protected* data;
    struct cellProtected* next;
} CellProtected;

Signature* init_signature(long* content, size_t size);
Signature* sign(char* mess, Key* sKey);
char* signature_to_str(Signature* sgn);
Signature* str_to_signature(char* str);

Protected* init_protected(Key* pKey, char* mess, Signature* sgn);
int verify(Protected* pr);

void freeSignature(Signature* signature);
void freeProtected(Protected* protected);
void delete_cell_key(CellKey* cellKey);
void delete_list_keys(CellKey* cellKey);

char* protected_to_str(Protected* p);
Protected* str_to_protected(char* str);

CellKey* create_cell_key(Key* key);
CellKey* prependKey(Key* key, CellKey* list);

void generate_random_data(int nv, int nc);
CellKey* read_public_keys(char* filename);

void print_list_keys(CellKey* list);

CellProtected* create_cell_protected(Protected* pr);
CellProtected* prependProtected(Protected* protected, CellProtected* list);

CellProtected* read_protected();

void print_list_protected(CellProtected* list);

#endif //PROJETSTR_VOTING_H
