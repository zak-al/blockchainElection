#include "Hash.h"
#include "primalite.h"
#include <openssl/sha.h>
#include <stdlib.h>
#include <string.h>

void DEBUG_printHashHex(const unsigned char* hash) {
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        printf("%02x", hash[i]);
    }

    printf("\n");
}

int hashEqual(unsigned char* a, unsigned char* b) {
    if (a == b) return TRUE;
    if (!a || !b) return FALSE;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        if (a[i] != b[i]) {
            return FALSE;
        }
    }

    return TRUE;
}

unsigned char* duplicateHash(const unsigned char* hash) {
    if (!hash) {
        return NULL;
    }

    unsigned char* res = malloc(SHA256_DIGEST_LENGTH * sizeof(char));

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        res[i] = hash[i];
    }

    return res;
}

char* hashToStr(const unsigned char* hash) {
    if (hash == NULL) {
        return strdup("(null)");
    }

    char* res = malloc(4 * SHA256_DIGEST_LENGTH * sizeof(char));
    int j = 0;

    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        char b[5];
        sprintf(b, "%02x", hash[i]);

        res[j++] = b[0];
        res[j++] = b[1];
    }
    res[j] = '\0';
    res = realloc(res, (strlen(res) + 1) * sizeof(char));

    return res;
}

void writeHash(const unsigned char* hash, FILE* file) {
    if (!hash) {
        fprintf(file, "(null)");
        return;
    }

    char* str = hashToStr(hash);

    fprintf(file, "%s", str);

    free(str);
}

unsigned char* readHash(const char* hash) {
    unsigned char* res = calloc(SHA256_DIGEST_LENGTH, sizeof(unsigned char));
    if (!res) {
        fprintf(stderr, "[readHash] Erreur lors de l'allocation de la mémoire");
        return NULL;
    }

    for (int i = 0; i < 2 * SHA256_DIGEST_LENGTH; i += 2) {
        if (hash[i] == '0') {
            unsigned char buf[2] = {hash[i + 1], 0};
            res[i / 2] = (unsigned char) strtol((char*) buf, NULL, 16);
        } else {
            unsigned char buf[3] = {hash[i], hash[i + 1]};
            res[i / 2] = (unsigned char) strtol((char*) buf, NULL, 16);
        }
    }

    return res;
}
