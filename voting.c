#include "voting.h"

int signaturesEqual(Signature* s1, Signature* s2) {
    if (s1->length != s2->length) return FALSE;

    for (int i = 0; i < s1->length; ++i) {
        if (s1->content[i] != s2->content[i]) return FALSE;
    }

    return TRUE;
}

int protectedEqual(Protected* p1, Protected* p2) {
    return strcmp(p1->message, p2->message) == 0 && keysEqual(p1->votersPublicKey, p2->votersPublicKey) &&
            signaturesEqual(p1->signature, p2->signature);
}

long* copy_long_array(long* array, size_t size) {
    long* copy = malloc(size * sizeof(long));
    for (size_t i = 0; i < size; ++i) {
        copy[i] = array[i];
    }

    return copy;
}

Signature* init_signature(long* content, size_t size) {
    Signature* signature = malloc(sizeof(Signature));
    if (!signature) {
        fprintf(stderr, "[init_signature] : erreur lors de l'allocation de la signature :(\n");
        return NULL;
    }

    signature->content = copy_long_array(content, size);
    signature->length = size;

    return signature;
}

Signature* copySignature(Signature* signature) {
    return init_signature(signature->content, signature->length);
}

Signature* sign(char* mess, Key* sKey) {
    long* signature_content = encrypt(mess, sKey->val, sKey->n);
    Signature* res = init_signature(signature_content, strlen(mess));
    free(signature_content);
    return res;
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

Protected* init_protected(Key* votersPublicKey, char* mess, Signature* sgn) {
    Protected* result = malloc(sizeof(Protected));

    if (!result) {
        fprintf(stderr, "[init_protected] Erreur lors de l'allocation de Protected :(\n");
        return NULL;
    }

    result->signature = copySignature(sgn);
    result->message = strdup(mess);
    result->votersPublicKey = copyKey(votersPublicKey);

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

    sprintf(p_res, "%s:%s:%s\n", str_key, p->message, str_sgn);

    free(str_key);
    free(str_sgn);

    p_res = realloc(p_res, (strlen(p_res) + 1) * sizeof(char));

    return p_res;
}

Protected* str_to_protected(char* str) {
    // todo tester
    char str_key[256];
    char str_sgn[256];
    char str_msg[256];
    sscanf(str, "%[^:]:%[^:]:%s", str_key, str_msg, str_sgn);

    Key *key = str_to_key(str_key);
    Signature *sgn = str_to_signature(str_sgn);

    Protected* res = init_protected(key, str_msg, sgn);

    freeKey(key);
    freeSignature(sgn);

    return res;
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
        fprintf(stderr, "[create_cell_key] Erreur lors de l'allocation de la mémoire :(\n");
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
        fprintf(stderr, "[prependKey] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellKey->next = list;
    return cellKey;
}

CellKey* read_public_keys(char* filename) {
    // todo tester
    CellKey* list = NULL;

    FILE* file = fopen(filename, "r");
    char str[256];
    while (fgets(str, 255, file)) {
        char public[256];
        sscanf(str, "%[^;]", public);
        Key* key = str_to_key(public);
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
        fprintf(stderr, "[create_cell_protected] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    cellProtected->data = pr;
    cellProtected->next = NULL;

    return cellProtected;
}

CellProtected* prependProtected(Protected* protected, CellProtected* list) {
    CellProtected* cellProtected = create_cell_protected(protected);
    if (!cellProtected) {
        fprintf(stderr, "[prependProtected] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
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

void generate_random_data(int nv, int nc) {
    FILE* fkeys = fopen("tab.txt","w+");
    FILE* candidatesFile = fopen("candidates.txt", "w+");
    FILE* declarationsFile = fopen("declarations.txt", "w");

    HashTable* candidates = initHashTable(nc);
    int* candidateNumbers = malloc(nc * sizeof(int));
    Key** voterIdxToPublicKey = malloc(nc * sizeof(Key*));
    Key** voterIdxToSecretKey = malloc(nc * sizeof(Key*));

    // La boucle ci-dessous tire nc candidats distincts et les ajoute à la table `candidates`.
    for (int i = 0; i < nc; ++i) {
        int idx;
        do {
            idx = rand() % nc;
        } while (hashTableContains(candidates, idx));
        add(candidates, idx, NULL);
        candidateNumbers[i] = idx;
    }

    printf("1.\n");

    /*
     * Création des clés de chaque électeur et enregistrement des clés publiques.
     */
    for (int i = 0; i < nv; ++i) {
        Key* publicKey = malloc(sizeof(Key));
        Key* secretKey = malloc(sizeof(Key));

        init_pair_keys(publicKey, secretKey, 3, 7);
        fprintf(fkeys, "%s, %s\n", key_to_str(publicKey), key_to_str(secretKey));

        /*if (hashTableContains(candidates, i)) {
            set(candidates, i, publicKey);
            fprintf(candidatesFile, "%s", key_to_str(publicKey));
        }*/

        voterIdxToPublicKey[i] = publicKey;
        voterIdxToSecretKey[i] = secretKey;
    }

    printf("2.\n");

    /*
     * Vote.
     * Pour chaque électeur, on choisit aléatoirement un indice entre 0 et (nc - 1),
     * qui correspond à l'indice d'un candidat.
     * On remplit ainsi le fichier de déclarations.
     */
    for (int i = 0; i < nv; ++i) {
        Key* votersPublicKey = voterIdxToPublicKey[i];
        Key* votersPrivateKey = voterIdxToSecretKey[i];

        int candidatesIdx = rand() % nc;
        Key* candidatesPublicKey = voterIdxToPublicKey[candidateNumbers[candidatesIdx]];

        // Message:
        char* candidatesPublicKeyStr = key_to_str(candidatesPublicKey);

        Signature* signature = sign(candidatesPublicKeyStr, votersPrivateKey);
        Protected* protected = init_protected(votersPublicKey, candidatesPublicKeyStr, signature);
        fprintf(declarationsFile, "%s\n", protected_to_str(protected));

        freeSignature(signature);
        freeProtected(protected);
    }

    printf("3.\n");

    fclose(fkeys);
    fclose(candidatesFile);
    fclose(declarationsFile);
    free(candidateNumbers);
    freeHashTable(candidates);

    for (int i = 0; i < nv; ++i) {
        freeKey(voterIdxToPublicKey[i]);
        freeKey(voterIdxToSecretKey[i]);
    }
    free(voterIdxToPublicKey);
}
