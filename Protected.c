#include "Protected.h"
#include <stdio.h>

int protectedEqual(Protected* p1, Protected* p2) {
    return strcmp(p1->message, p2->message) == 0 && keysEqual(p1->votersPublicKey, p2->votersPublicKey) &&
           signaturesEqual(p1->signature, p2->signature);
}

// QUESTION 3.10
/**
 * Alloue dynamiquement une instance de Protected et l'initialise avec les valeurs passées en paramètre.
 * @param votersPublicKey
 * @param mess
 * @param sgn
 * @return
 */
Protected* initProtected(const Key* votersPublicKey, char* mess, Signature* sgn) {
    Protected* result = malloc(sizeof(Protected));

    if (!result) {
        fprintf(stderr, "[initProtected] Erreur lors de l'allocation de Protected :(\n");
        return NULL;
    }

    result->signature = copySignature(sgn);
    result->message = strdup(mess);
    result->votersPublicKey = copyKey(votersPublicKey);

    return result;
}

// QUESTION 3.11
/**
 * Renvoie TRUE si la signature contenue dans la déclaration correspond bien au message encodée par l'électeur.
 * @param pr
 * @return
 */
int verify(Protected* pr) {
    char* decrypted = decrypt(pr->signature->content,
                              pr->signature->length,
                              pr->votersPublicKey->val,
                              pr->votersPublicKey->n);
    int res = strcmp(decrypted, pr->message) == 0 ? TRUE : FALSE;
    free(decrypted);
    return res;
}

// QUESTION 3.12
char* protectedToStr(Protected* p) {
    if (p == NULL) {
        return NULL;
    }

    char* p_res = malloc(1024 * sizeof(char));
    if (!p_res) {
        fprintf(stderr, "[protectedToStr] Erreur lors de l'allocation :(\n");
        return NULL;
    }

    char* str_key = keyToStr(p->votersPublicKey);
    char* str_sgn = signature_to_str(p->signature);

    sprintf(p_res, "%s:%s:%s", str_key, p->message, str_sgn);

    free(str_key);
    free(str_sgn);

    p_res = realloc(p_res, (strlen(p_res) + 1) * sizeof(char));

    return p_res;
}

Protected* strToProtected(char* str) {
    char str_key[256];
    char str_sgn[256];
    char str_msg[256];
    sscanf(str, "%[^:]:%[^:]:%s", str_key, str_msg, str_sgn);

    Key* key = strToKey(str_key);
    Signature* sgn = str_to_signature(str_sgn);

    Protected* res = initProtected(key, str_msg, sgn);

    freeKey(key);
    freeSignature(sgn);

    return res;
}

void freeProtected(Protected* protected) {
    if (!protected) return;

    free(protected->message);
    freeSignature(protected->signature);
    freeKey(protected->votersPublicKey);

    free(protected);
}

Protected* copyProtected(Protected* protected) {
    Protected* copy = malloc(sizeof(Protected));
    if (!copy) {
        fprintf(stderr, "[copyProtected] Erreur lors l'allocation :(\n");
        return NULL;
    }

    copy->votersPublicKey = copyKey(protected->votersPublicKey);
    copy->signature = copySignature(protected->signature);
    copy->message = strdup(protected->message);

    return copy;
}
