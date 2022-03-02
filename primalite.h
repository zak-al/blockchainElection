//
// Created by Zakarie Aloui on 23/02/2022.
//

#ifndef PROJETSTR_PRIMALITE_H
#define PROJETSTR_PRIMALITE_H

#define TRUE (1 == 1)
#define FALSE (!TRUE)
#define mod %

int is_prime_naive(long p);
long modpow_naive(long base, long exponent, long n);
long modpow(long base, long exponent, long n);
int witness(long a, long b, long d, long p);
long rand_long(long low, long up);

int is_prime_miller(long p, int k);
long random_prime_number(int low_size, int up_size, int k);

#endif //PROJETSTR_PRIMALITE_H
