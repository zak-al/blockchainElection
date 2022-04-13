#include "Protected.h"
#include <stdio.h>

int protectedEqual(Protected *p1, Protected *p2) {
    return strcmp(p1->message, p2->message) == 0 && keysEqual(p1->votersPublicKey, p2->votersPublicKey) &&
           signaturesEqual(p1->signature, p2->signature);
}

Protected *init_protected(Key *votersPublicKey, char *mess, Signature *sgn) {
    Protected *result = malloc(sizeof(Protected));

    if (!result) {
        fprintf(stderr, "[init_protected] Erreur lors de l'allocation de Protected :(\n");
        return NULL;
    }

    result->signature = copySignature(sgn);
    result->message = strdup(mess);
    result->votersPublicKey = copyKey(votersPublicKey);

    return result;
}

int verify(Protected *pr) {
    char *decrypted = decrypt(pr->signature->content,
                              pr->signature->length,
                              pr->votersPublicKey->val,
                              pr->votersPublicKey->n);
    int res = strcmp(decrypted, pr->message) == 0 ? TRUE : FALSE;
    free(decrypted);
    return res;
}

char *protected_to_str(Protected *p) {
    char *p_res = malloc(256 * sizeof(char));
    if (!p_res) {
        fprintf(stderr, "[protected_to_str] Erreur lors de l'allocation :(\n");
        return NULL;
    }
    char *str_key = key_to_str(p->votersPublicKey);
    char *str_sgn = signature_to_str(p->signature);

    sprintf(p_res, "%s:%s:%s", str_key, p->message, str_sgn);

    free(str_key);
    free(str_sgn);

    p_res = realloc(p_res, (strlen(p_res) + 1) * sizeof(char));

    return p_res;
}

Protected *str_to_protected(char *str) {
    char str_key[256];
    char str_sgn[256];
    char str_msg[256];
    sscanf(str, "%[^:]:%[^:]:%s", str_key, str_msg, str_sgn);

    Key *key = str_to_key(str_key);
    Signature *sgn = str_to_signature(str_sgn);

    Protected *res = init_protected(key, str_msg, sgn);

    freeKey(key);
    freeSignature(sgn);

    return res;
}

void freeProtected(Protected *protected) {
    if (protected) {
        free(protected->message);
        freeSignature(protected->signature);
        freeKey(protected->votersPublicKey);
    }

    free(protected);
}

Protected *copyProtected(Protected *protected) {
    Protected *copy = malloc(sizeof(Protected));
    if (!copy) {
        fprintf(stderr, "[copyProtected] Erreur lors l'allocation :(\n");
        return NULL;
    }

    copy->votersPublicKey = copyKey(protected->votersPublicKey);
    copy->signature = copySignature(protected->signature);
    copy->message = strdup(protected->message);

    return copy;
}
