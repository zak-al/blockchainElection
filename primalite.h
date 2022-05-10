#ifndef PROJETSTR_PRIMALITE_H
#define PROJETSTR_PRIMALITE_H

#define TRUE (1 == 1)
#define FALSE (!TRUE)
#define mod %

int isPrimeNaive(long p);

long modpowNaive(long base, long exponent, long n);

long modpow(long base, long exponent, long n);

int witness(long a, long b, long d, long p);

long randLong(long low, long up);

int isPrimeMiller(long p, int k);

long randomPrimeNumber(int low_size, int up_size, int k);

#endif //PROJETSTR_PRIMALITE_H
