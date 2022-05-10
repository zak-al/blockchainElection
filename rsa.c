#include <stdio.h>
#include "rsa.h"

// QUESTION 2.1
/**
 * \~french
 * @brief Forme la clé publique (s, n) et la clé privée (u, n) en fonction de deux nombres premiers p et q.
 * @param p un grand nombre premier, généré aléatoirement
 * @param q un autre grand nombre premier, généré aléatoirement, différent de p
 * @param n pointeur qui prendra la valeur du résidu, i.e. p*q
 * @param s pointeur qui prendra le paramètre s de la clé publique
 * @param u pointeur qui prendra la paramètre u de la clé privée, i.e. l'inverse modulo n de s
 * \~english
 * @brief Determines the public key (s, n) and the private key (u, n) in terms of two prime numbers passed as parameters, p and q.
 * @param p a randomly-generated, large prime number
 * @param q another randomly-generated large prime number, different to p
 * @param n pointer that is to take the value of the modulo, n, i.e. p*q
 * @param s pointer that is to take the parameter s of the public key
 * @param u pointer that is to take the parameter u of the private key, i.e. the multiplicative inverse of s modulo n
 */
void generateKeyValues(long p, long q, long *n, long *s, long *u) {
    *n = p * q;
    long t = (p - 1) * (q - 1);
    long _s;
    long _ = 0;
    do {
        _s = randLong(0, t);
    } while (extendedGcd(_s, t, u, &_) != 1);

    *s = _s;

    if (*u < 0) {
        *u += t;
    }
}

// QUESTION 2.2
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
long *encrypt(const char *string, long s, long n) {
    size_t len = strlen(string);

    long *res = malloc(len * sizeof(long));
    if (!res) {
        fprintf(stderr, "erreur de l'allocation\n");
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        res[i] = modpow((long) string[i], s, n);
    }

    return res;
}

// QUESTION 2.3
/**
 * \~french
 * @brief Déchiffre une chaîne de caractères
 * @param string représentation de la chaîne chiffrée, comme renvoyée par la fonction encrypt
 * @param size nombre d'éléments dans le tableau chaine
 * @param u paramètre u de la clé privée
 * @param n paramètre n (modulo)
 * @return la valeur en clair de la chaîne chiffrée contenue dans le premier paramètre, terminée par le caractère nul
 *
 * \~english
 * @brief Decrypts a string
 * @param string representation of an encrypted string, as returned by the function encrypt
 * @param size number of elements in the parameter string
 * @param u parameter u of the private key
 * @param n parameter n (modulo)
 * @return a null-terminated string corresponding to the uncrypted value of the string contained in the first parameter
 */
char *decrypt(const long *string, size_t size, long u, long n) {
    char *res = malloc((size + 1) * sizeof *res);

    if (!res) {
        fprintf(stderr, "erreur de l'allocation\n");
        return NULL;
    }

    for (size_t i = 0; i < size; ++i) {
        res[i] = (char) modpow(string[i], u, n);
    }

    res[size] = '\0';

    return res;
}

// QUESTION 3.2
/**
 * Initialise une clé déjà allouée avec les valeurs passées en paramètre.
 * @param key
 * @param val
 * @param n
 */
void initKey(Key *key, long val, long n) {
    key->val = val;
    key->n = n;
}

/**
 * @brief Teste l'égalité champ à champ entre deux clés.
 * Si les deux pointeurs sont nuls alors la fonction renvoie TRUE.
 * @param x
 * @param y
 */
int keysEqual(const Key* x, const Key* y) {
    if (x == y) return TRUE;
    return (x->n == y->n && x->val == y->val);
}

/**
 * @brief Copies a key.
 * @param key A key.
 * @return A dynamically-allocated copy of `key`.
 */
Key *copyKey(const Key *key) {
    Key *copy = malloc(sizeof(Key));
    if (!copy) {
        fprintf(stderr, "[copyKey] Erreur lors l'allocation :(\n");
        return NULL;
    }

    copy->n = key->n;
    copy->val = key->val;
    return copy;
}

// QUESTION 3.3
/**
 * Initialise un couple (clé publique, clé privée) avec des valeurs appropriées, choisies aléatoirement.
 * @param pKey
 * @param sKey
 * @param low_size Minimum de bits dans les paramètres des clés.
 * @param up_size Maximum de bits dans les paramètre des clés.
 */
void initPairKeys(Key *pKey, Key *sKey, int low_size, int up_size) {
    long pub_val, priv_val, n;
    long p = randomPrimeNumber(low_size, up_size, 5);
    long q;
    do {
        q = randomPrimeNumber(3, 7, 5000);
    } while (q == p);

    generateKeyValues(p, q, &n, &pub_val, &priv_val);
    initKey(pKey, pub_val, n);
    initKey(sKey, priv_val, n);
}

// QUESTION 3.4
char *keyToStr(Key *key) {
    char *repr = malloc(64);
    if (!repr) {
        fprintf(stderr, "[keyToStr] Erreur lors l'allocation de la représentation de la clé :(\n");
        return NULL;
    }

    sprintf(repr, "(%lx, %lx)", key->val, key->n);
    repr = realloc(repr, (strlen(repr) + 1) * sizeof(char));
    return repr;
}

Key *strToKey(char *repr) {
    long val, n;
    sscanf(repr, "(%lx, %lx)", &val, &n);
    Key *key = malloc(sizeof(Key));
    if (!key) {
        fprintf(stderr, "[strToKey] Erreur lors l'allocation de la clé :(\n");
        return NULL;
    }

    initKey(key, val, n);
    return key;
}

long extendedGcd(long s, long t, long *u, long *v) {
    if (s == 0) {
        *u = 0;
        *v = 1;
        return t;
    }

    long uPrim, vPrim;
    long gcd = extendedGcd(t mod s, s, &uPrim, &vPrim);

    *u = vPrim - (t / s) * uPrim;
    *v = uPrim;

    return gcd;
}


void freeKey(Key *key) {
    free(key);
}
