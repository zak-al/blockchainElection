//
// Created by Zakarie Aloui on 09/03/2022.
//

#include "voting.h"

Signature* init_signature(long* content, size_t size) {
    Signature* signature = malloc(sizeof(Signature));
    if (!signature) {
        fprintf(stderr, "[init_signature] : erreur lors de l'allocation de la signature :(");
        return NULL;
    }

    signature->content = content;
    signature->length = size;

    return signature;
}

Signature* sign(char* mess, Key* sKey) {
    long* signature_content = encrypt(mess, sKey->val, sKey->n);
    return init_signature(signature_content, strlen(mess));
}

char* signature_to_str(Signature* sgn) {
    char *result = malloc(10 * sgn->length * sizeof(char));
    result[0] = '#';
    int pos = 1;
    char buffer[156];
    for (int i = 0; i < sgn->length; ++i) {
        sprintf(buffer, "%lx", sgn->content[i]);
        for (int j = 0; j < strlen(buffer); ++j) {
            result[pos] = buffer[j];
            ++pos;
        }
        result[pos] = '#';
        ++pos;
    }

    result[pos] = '\0';
    result = realloc(result, (pos + 1) * sizeof(char));
    return result;
}

Signature* str_to_signature(char* str) {
    size_t len = strlen(str);
    long *content = (long *) malloc(sizeof(long) * len);
    int num = 0;
    char buffer[256];
    int pos = 0;
    for (size_t i = 0; i < len; i++) {
        if (str[i] != '#') {
            buffer[pos] = str[i];
            pos = pos + 1;
        } else {
            if (pos != 0) {
                buffer[pos] = '\0';
                sscanf(buffer, "%lx", &(content[num]));
                num = num + 1;
                pos = 0;
            }
        }
    }
    content = realloc(content, num * sizeof(long));
    return init_signature(content, num);
}

Protected* init_protected(Key* pKey, char* mess, Signature* sgn) {
    Protected* result = malloc(sizeof(Protected));

    if (!result) {
        fprintf(stderr, "[init_protected] Erreur lors de l'allocation de Protected :(\n");
        return NULL;
    }

    result->signature = sgn;
    result->message = strdup(mess);
    result->votersPublicKey = pKey;

    return result;
}

int verify(Protected* pr) {
    char* decrypted = decrypt(pr->signature->content,
                              pr->signature->length,
                              pr->votersPublicKey->val,
                              pr->votersPublicKey->n);
    int res = strcmp(decrypted, pr->message) == 0 ? TRUE : FALSE;
    free(decrypted);
    return res;
}

char* protected_to_str(Protected* p){
    char *p_res = malloc(256 * sizeof(char));
    if (!p_res) {
        fprintf(stderr, "[protected_to_str] Erreur lors de l'allocation :(\n");
        return NULL;
    }
    char *str_key = key_to_str(p->votersPublicKey);
    char *str_sgn = signature_to_str(p->signature);

    sprintf(p_res, "%s, %s, %s\n", str_key, p->message, str_sgn);

    free(str_key);
    free(str_sgn);

    p_res = realloc(p_res, (strlen(p_res) + 1) * sizeof(char));

    return p_res;
}

Protected* str_to_protected(char* str){
    char str_key[256];
    char str_sgn[256];
    char str_msg[256];
    sscanf(str, "%s, %s, %s", str_key, str_msg, str_sgn);

    Key* key= str_to_key(str_key);
    Signature * sgn = str_to_signature(str_sgn);

    return init_protected(key,str_msg,sgn);
}

void generate_random_data(int nv, int nc) {
    FILE* fkeys = fopen("keys.txt","w+");
    FILE* fcans = fopen("candidates.txt","w+");
    FILE* fdecs = fopen("declarations.txt","w");

    //todo le tableau de hachage pour obtenir nc candidates à Zakarie
    char* pKeyC = NULL;
    char* sKeyC = NULL;
    Key* pKey = malloc(sizeof(Key));
    Key* sKey = malloc(sizeof(Key));
    int i;

    for(i=0; i<nv;i++){
        init_pair_keys(pKey, sKey,3,7);

        pKeyC = strdup(key_to_str(pKey));
        sKeyC = strdup(key_to_str(sKey));

        fprintf(fkeys, "%s, %s \n", pKeyC, sKeyC);
    }

    for(i=0; i<nc; i++){
        sscanf(fkeys, "%s, %s", pKeyC,sKeyC);
        fprintf(fcans, "%s \n",pKeyC);
    }

    free(pKeyC);
    free(sKeyC);
    freeKey(pKey);
    freeKey(sKey);

}


void freeSignature(Signature* signature) {
    if (signature) {
        free(signature->content);
    }
    free(signature);
}

void freeProtected(Protected* protected) {
    if (protected) {
        free(protected->message);
        freeSignature(protected->signature);
        freeKey(protected->votersPublicKey);
    }

    free(protected);
}



void delete_cell_key(CellKey* cellKey) {
    if (! cellKey) return;
    freeKey(cellKey->data);
    free(cellKey);
}

void delete_list_keys(CellKey* cellKey) {
    while (cellKey) {
        CellKey* next = cellKey->next;
        delete_cell_key(cellKey);
        cellKey = next;
    }
}

CellKey* create_cell_key(Key* key) {
    CellKey* cellKey = malloc(sizeof(CellKey));
    if (!cellKey) {
        fprintf(stderr, "[create_cell_key] Erreur lors de l'allocation de la mémoire :(");
        return NULL;
    }

    cellKey->data = key;
    cellKey->next = NULL;

    return cellKey;
}

/**
 * @brief Crée une nouvelle liste en ajoutant une cellule contenant le clé passée en paramètre en tête de la liste passée.
 * @param key Clé contenue dans la future tête de la liste.
 * @param list Liste à laquelle ajouter la nouvelle cellule.
 * @return Liste mise à jour.
 */
CellKey* prependKey(Key* key, CellKey* list) {
    CellKey* cellKey = create_cell_key(key);
    if (!cellKey) {
        fprintf(stderr, "[prependKey] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(");
        return list;
    }

    cellKey->next = list;
    return cellKey;
}

CellKey* read_public_keys(char* filename) {
    CellKey* list = NULL;

    FILE* file = fopen(filename, "r");
    char str[256];
    while (fgets(str, 255, file)) {
        Key* key = str_to_key(str);
        list = prependKey(key, list);
        freeKey(key);
        key = NULL;
    }

    return list;
}

void print_list_keys(CellKey* list) {
    while (list) {
        printf("%s\n", key_to_str(list->data));
        list = list->next;
    }
}

CellProtected* create_cell_protected(Protected* pr) {
    CellProtected* cellProtected = malloc(sizeof(CellProtected));

    if (!cellProtected) {
        fprintf(stderr, "[create_cell_protected] Erreur lors de l'allocation de la mémoire :(");
        return NULL;
    }

    cellProtected->data = pr;
    cellProtected->next = NULL;

    return cellProtected;
}

CellProtected* prependProtected(Protected* protected, CellProtected* list) {
    CellProtected* cellProtected = create_cell_protected(protected);
    if (!cellProtected) {
        fprintf(stderr, "[prependProtected] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(");
        return list;
    }

    cellProtected->next = list;
    return cellProtected;
}

CellProtected* read_protected() {
    CellProtected* list = NULL;

    FILE* file = fopen("declarations.txt", "w");
    char str[256];
    while (fgets(str, 255, file)) {
        Protected* protected = str_to_protected(str);
        list = prependProtected(protected, list);
        freeProtected(protected);
        protected = NULL;
    }

    return list;
}

void print_list_protected(CellProtected* list){
    while (list){
        printf("%s \n", protected_to_str(list->data));
        list = list->next;
    }
}

void delete_cell_protected(CellProtected* cellProtected){
    if (! cellProtected) return;
    freeProtected(cellProtected->data);
    free(cellProtected);
}

void delete_list_protected(CellProtected* cellProtected) {
    while (cellProtected) {
        CellProtected * next = cellProtected->next;
        delete_cell_protected(cellProtected);
        cellProtected = next;
    }
}

void delete_liste_fraude(CellProtected* cellProtected){
    while (cellProtected){
        CellProtected* next = cellProtected -> next;
        if(!verify(cellProtected->data)){
            delete_cell_protected(cellProtected);
        }
        cellProtected = next;
    }
}
