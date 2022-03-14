//
// Created by Zakarie Aloui on 09/03/2022.
//

#include "voting.h"

Signature* init_signature(long* content, size_t size) {
    Signature* signature = malloc(sizeof(signature));
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


Signature* str_to_siganture(char* str) {
    int len = strlen(str);
    long *content = (long *) malloc(sizeof(long) * len);
    int num = 0;
    char buffer[256];
    int pos = 0;
    for (int i = 0; i < len; i++) {
        if (str[i] != "#") {
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
    char* decrypted = decrypt(pr->signature->content, pr->signature->length, pr->votersPublicKey->val, pr->votersPublicKey->n);
    int res = strcmp(decrypted, pr->message) == 0 ? TRUE : FALSE;
    free(decrypted);
    return res;
}
