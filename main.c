#include <stdio.h>
#include <time.h>
#include <limits.h>
#include "partie1.h"
#include <math.h>

long gros_premier() {
    long p = 3127878;
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
            //printf("max : %d\n", max);
            ++p;
        }

        if (n mod 5000 == 0) {
            printf("%ld\n", p);
        }

        ++p;
        ++n;
    }

    printf("Le plus grand nombre premier verifié en moins de deux secondes est %ld, en %f secondes.\n", max, runtime);

    return max;
}

void test_modpow() {
    clock_t start,end;
    double tempsCPU;
    FILE* f = fopen("sortie_pow.txt", "w");

    for(int i = 1; i< 100; i++){
        long a = LONG_MAX *i/100;
        long b = LONG_MAX *(100 / (100+i));

        start = clock();
        modpow_naive(b, a, i);
        end = clock();
        tempsCPU = ((double)end-start)/CLOCKS_PER_SEC;
        fprintf(f,"%d %f", i, tempsCPU);

        start = clock();
        modpow(b, a, i);
        end = clock();
        tempsCPU = ((double)end-start)/CLOCKS_PER_SEC;
        fprintf(f,"%f\n", tempsCPU);

    }
}

int main() {
    //int p = gros_premier();

    /*long a = LONG_MAX - 76;
    long b = LONG_MAX / 6 + 1;
    int n = 100;

    printf("%ld\n", modpow_naive(b, a, n));
    printf("%ld\n", modpow(b, a, n));*/

    //test_modpow();

    gros_premier();

    return 0;
}
