#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "primalite.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#define NB_TESTS 10000

long gros_premier() {
    //long p = 7926200;
    long p = 239316881;
    double runtime = 0;
    long max = p;
    long n = 1;

    while (runtime <= 2) {
        clock_t start, end;
        int res;

        start = clock();
        res = is_prime_naive(p);
        end = clock();

        runtime = ((double) (end - start)) / CLOCKS_PER_SEC;

        if (res == TRUE && runtime <= 2) {
            max = p;
            printf("\tFOUND PRIME %ld IN %f\n", p, runtime);
        }

        p += 1;
        ++n;
    }

    printf("Le plus grand nombre premier verifié en moins de deux secondes est %ld, en %f secondes.\n", max, runtime);

    return max;
}

void test_modpow() {
    clock_t start, end;
    double tempsCPU;
    FILE* f = fopen("sortie_pow.txt", "w");

    long b = 12345432;
    long n = 150;

    // Échauffement
    for (int i = 1; i < 100; ++i) {
        modpow(b, b * i, n);
    }

    long i;
    for(i = 2; i < 2 * NB_TESTS; i += 2) {
        long a = (LONG_MAX / (2 * NB_TESTS)) * i;
        /*start = clock();
        modpow_naive(b, a, n);
        end = clock();
        tempsCPU = ((double) (end - start))/CLOCKS_PER_SEC;
        fprintf(f,"%d %f", i, tempsCPU);*/

        start = clock();
        modpow(b, a, n);
        end = clock();
        tempsCPU = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(f, "%ld %f ", i, tempsCPU);

        start = clock();
        modpow(b, a + 1, n);
        end = clock();
        tempsCPU = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(f, "%f\n", tempsCPU);
    }

    fclose(f);
}

void test_faux_positifs() {
    long p = 239317341;
    clock_t deb, fin;
    double tps;
    int f = 0;
    deb = clock();
    for (long long i = 0; i < 10000000000; ++i) {
        if (is_prime_miller(p, 1)) {
            f++;
        }
    }
    fin = clock();
    printf("%d faux positifs\n", f);
    tps = ((double) (fin - deb)) / CLOCKS_PER_SEC;
    printf("en %f secondes.\n", tps);
}

int main() {
    srand(time(NULL));
    //test_modpow();
    //gros_premier();
    //printf("%ld\n", random_prime_number(31, 31, 4));
    test_faux_positifs();
    return 0;
}
