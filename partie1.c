//
// Created by Zakarie Aloui on 23/02/2022.
//

#include "partie1.h"

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
