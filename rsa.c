//
// Created by Zakarie Aloui on 01/03/2022.
//

#include <stdio.h>
#include "rsa.h"

size_t max(long x, long y) {
    return x < y ? y : x;
}

long extended_gcd(long s, long t, long* u, long* v) {
    if (s == 0) {
        *u = 0;
        *v = 1;
        return t;
    }

    long uPrim, vPrim;
    long gcd = extended_gcd(t mod s, s, &uPrim, &vPrim);

    *u = vPrim - (t / s) * uPrim;
    *v = uPrim;

    return gcd;
}

/**
 * \~french
 * @brief Forme la clé publique (s, n) et la clé privée (u, n) en fonction de deux nombres premiers p et q.
 * @param p un grand nombre premier, généré aléatoirement
 * @param q un autre grand nombre premier, généré aléatoirement, différent de p
 * @param n pointeur qui prendra la valeur du résidu, i.e. p*q
 * @param s pointeur qui prendra le paramètre s de la clé publique
 * @param u pointeur qui prendra la paramètre u de la clé privée, i.e. l'inverse modulo n de s
 * \~english
 * @brief Determines the public key (s, n) and the private key (u, n) in terms of to prime numbers passed as parameters, p and q.
 * @param p a randomly-generated, large prime number
 * @param q another randomly-generated large prime number, different to p
 * @param n pointer that is to take the value of the modulo, n, i.e. p*q
 * @param s pointer that is to take the parameter s of the public key
 * @param u pointer that is to take the parameter u of the private key, i.e. the multiplicative inverse of s modulo n
 */
void generate_key_values(long p, long q, long* n, long* s, long* u) {
    *n = p * q;
    long t = (p - 1) * (q - 1);
    long _s;
    long _ = 0;
    do {
        _s = rand_long(0, t);
    } while (extended_gcd(_s, t, u, &_) != 1);

    *s = _s;

    if (*u < 0) {
        *u += t;
    }
}

/**
 * \~french
 * @brief Chiffre une chaîne de caractères
 * @param string chaine de caractères à chiffrer
 * @param s clé publique
 * @param n paramètre n de la clé publique (modulo)
 * @return un tableau de long contenant la version chiffrée de chaque caractère
 *
 * \~english
 * @brief Encrypts a string
 * @param string string to encrypt
 * @param s public key
 * @param n parameter n of the public key (modulo)
 * @return a long array where each cell is the encrypted version of the corresponding character in the input string
 */
long* encrypt(const char* string, long s, long n) {
    size_t len = strlen(string);

    long* res = malloc(len * sizeof(long));
    if (!res) {
        fprintf(stderr,"erreur de l'allocation\n");
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        res[i] = modpow((long) string[i], s, n);
    }

    return res;
}

/**
 * \~french
 * @brief Déchiffre une chaîne de caractères
 * @param string représentation de la chaîne chiffrée, comme renvoyée par la fonction encrypt
 * @param size nombre d'éléments dans le tableau chaine
 * @param u paramètre u de la clé privée
 * @param n paramètre n (modulo)
 * @return la valeur en clair de la chaîne chiffrée contenue dans le premier paramètre, terminée par le caractère nul
 *
 * \~french
 * @brief Decrypts a string
 * @param string representation of an encrypted string, as returned by the function encrypt
 * @param size number of elements in the parameter string
 * @param u parameter u of the private key
 * @param n parameter n (modulo)
 * @return a null-terminated string corresponding to the uncrypted value of the string contained in the first parameter
 */
char* decrypt(const long* string, size_t size, long u, long n) {
    char* res = malloc((size + 1) * sizeof *res);

    if (!res) {
        fprintf(stderr,"erreur de l'allocation\n");
        return NULL;
    }

    for (size_t i = 0; i < size; ++i) {
        res[i] = (char) modpow(string[i], u, n);
    }

    res[size] = '\0';

    return res;
}

void init_key(Key* key, long val, long n) {
    key->val = val;
    key->n = n;
}

// todo questionnement sur le type de low_size et up_size...
void init_pair_keys(Key* pKey, Key* sKey, int low_size, int up_size) {
    long pub_val, priv_val, n;
    long p = random_prime_number(low_size, up_size, 5);
    long q;
    do {
        q = random_prime_number(3, 7, 5000);
    } while (q == p);

    generate_key_values(p, q, &n, &pub_val, &priv_val);
    init_key(pKey, pub_val, n);
    init_key(sKey, priv_val, n);
}

char* key_to_str(Key* key) {
    // Todo tester
    // Le paramètre de malloc permet de faire une estimation de la taille espérée de la chaîne.
    char* repr = malloc((max(key->n / 16 + 1, key->val / 16 + 1) + 5) * 2);
    if (!repr) {
        fprintf(stderr, "[] Erreur lors l'allocation de la représentation de la clé :(\n");
        return NULL;
    }

    sprintf(repr, "(%lx, %lx)", key->val, key->n);
    realloc(repr, strlen(repr) + 1);

    return repr;
}

Key* str_to_key(char* repr) {
    // todo tester
    long val, n;
    sscanf(repr, "(%lx, %lx)", &val, &n);
    Key* key = malloc(sizeof(Key));
    if (!key) {
        fprintf(stderr, "[str_to_key] Erreur lors l'allocation de la clé :(\n");
        return NULL;
    }

    init_key(key, val, n);
    return key;
}

void freeKey(Key* key) {
    free(key);
}
