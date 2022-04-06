//
// Created by Zakarie Aloui on 30/03/2022.
//

#include <stdio.h>
#include "Signature.h"

long *copyLongArray(const long *array, size_t size) {
    long *copy = malloc(size * sizeof(long));
    if (!copy) {
        fprintf(stderr, "[copyLongArray] Erreur lors l'allocation :(\n");
        return NULL;
    }

    for (size_t i = 0; i < size; ++i) {
        copy[i] = array[i];
    }

    return copy;
}

void printLongVector(long *result, size_t size) {
    for (size_t i = 0; i < size - 1; ++i) {
        printf("%lx; ", result[i]);
    }

    if (size != 0) {
        printf("%lx", result[size - 1]);
    }

    printf("]");
}

int signaturesEqual(Signature *s1, Signature *s2) {
    if (s1->length != s2->length) return FALSE;

    for (int i = 0; i < s1->length; ++i) {
        if (s1->content[i] != s2->content[i]) return FALSE;
    }

    return TRUE;
}

Signature *initSignature(const long *content, size_t size) {
    Signature *signature = malloc(sizeof(Signature));
    if (!signature) {
        fprintf(stderr, "[initSignature] : erreur lors de l'allocation de la signature :(\n");
        return NULL;
    }

    signature->content = copyLongArray(content, size);
    signature->length = size;

    return signature;
}

Signature *copySignature(Signature *signature) {
    return initSignature(signature->content, signature->length);
}

Signature *sign(const char *mess, const Key *sKey) {
    long *signature_content = encrypt(mess, sKey->val, sKey->n);
    Signature *res = initSignature(signature_content, strlen(mess));
    free(signature_content);
    return res;
}

char *signature_to_str(Signature *sgn) {
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

Signature *str_to_signature(char *str) {
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
    return initSignature(content, num);
}

void freeSignature(Signature *signature) {
    if (signature) {
        free(signature->content);
    }
    free(signature);
}

