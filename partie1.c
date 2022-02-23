//
// Created by Zakarie Aloui on 23/02/2022.
//

#include "partie1.h"

int is_prime_naive(long p) {
    if (p < 2) {
        return FALSE;
    }

    if (p % 2 == 0) {
        return FALSE;
    }

    for (int i = 3; i < p; ++i) {
        if (p % i == 0) return FALSE;
    }

    return TRUE;
}

long modpow_naive(long base, long exponent, long n) {
    long long res = 1;
    while (exponent > 0) {
        res = (res * base) % n;
        --exponent;
    }

    return res;
}

long modpow(long base, long exponent, long n) {

}
