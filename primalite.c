//
// Created by Zakarie Aloui on 23/02/2022.
//

#include <stdlib.h>
#include <printf.h>
#include <string.h>
#include "primalite.h"

int is_prime_naive(long p) {
    if (p < 2) {
        return FALSE;
    }

    if (p mod 2 == 0) {
        return FALSE;
    }

    for (int i = 3; i < p; ++i) {
        if (p mod i == 0) return FALSE;
    }

    return TRUE;
}

long modpow_naive(long base, long exponent, long n) {
    long res = 1;
    while (exponent > 0) {
        res = (res * base) mod n;
        --exponent;
    }

    return res;
}

long modpow(long base, long exponent, long n) {
    base = base mod n;
    long res = 1;

    while (exponent > 0) {
        if (exponent mod 2 != 0) {
            res = (res * base) mod n;
        }

        base = (base * base) mod n;
        exponent /= 2;
    }

    return res;
}

int witness(long a, long b, long d, long p) {
    long x = modpow(a,d,p);
    if (x == 1) {
        return 0;
    }
    for (long i = 0; i < b; i++) {
        if (x == p - 1) {
            return 0;
        }
        x = modpow(x, 2, p);
    }
    return 1;
}

long rand_long(long low, long up) {
    return rand() % (up - low + 1) + low;
}

int is_prime_miller(long p, int k) {
    if (p == 2) {
        return 1;
    }
    if (!(p & 1) || p <= 1) {
        //on verifie que p est impair et different de 1
        return 0;
    }
    //on determine b et d :
    long b = 0;
    long d = p - 1;
    while (!(d & 1)) { //tant que d n’est pas impair
        d = d/2;
        b=b+1;
    }
    // On genere k valeurs pour a, et on teste si c’est un temoin :
    long a;
    int i;
    for (i = 0; i < k; i++) {
        a = rand_long(2, p-1);
        if (witness(a, b, d, p)) {
            return 0;
        }
    }
    return 1;
}

long random_prime_number(int low_size, int up_size, int k) {
    long lo, hi;
    if (up_size > 30) up_size = 30;
    if (low_size > up_size) low_size = up_size;
    hi = (1 << (up_size + 1)) - 1;
    lo = 1 << low_size;

    long p;
    do {
        p = rand_long(lo, hi);
    } while (!is_prime_miller(p, k));

    return p;
}

long extended_gcd(long s, long t, long* u, long* v) {
    if (t == 0) {
        *u = 1;
        *v = 0;
        return s;
    }

    long uPrim, vPrim;
    long gcd = extended_gcd(t, s mod t, &uPrim, &vPrim);
    *u = uPrim;

    if (v) {
        *v = uPrim - (s / t) * vPrim;
    }

    return gcd;
}

void generate_key_values(long p, long q, long* n, long* s, long* u) {
    *n = p * q;
    long t = (p - 1) * (q - 1);
    long _s;
    long* _ = NULL;
    do {
        _s = rand_long(0, t);
        // todo vérifier pour _ dans l'appel à extended_gcd
    } while (extended_gcd(_s, t, u, _) == 1);

    *s = _s;
}

long* encrypt(const char* chaine, long s, long n) {
    size_t len = strlen(chaine);

    long* res = malloc(len * sizeof *res);
    if (!res) {
        return NULL;
    }

    for (size_t i = 0; i < len; ++i) {
        res[i] = modpow((long) chaine[i], s, n);
    }
}

char* decrypt(const long* chaine, size_t size, long u, long n) {
    char* res = malloc((size + 1) * sizeof *res);

    if (!res) return NULL;

    for (size_t i = 0; i < size; ++i) {
        res[i] = (char) modpow(chaine[i], u, n);
    }

    res[size] = '\0';

    return res;
}
