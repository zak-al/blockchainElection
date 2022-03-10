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

char* signature_to_str (Signature* sgn) {
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