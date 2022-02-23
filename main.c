#include <stdio.h>
#include <time.h>
#include "partie1.h"

int main() {
    int p = 3;
    double runtime = 0;
    int max = p;

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
        }

        ++p;
    }

    printf("Le plus grand nombre premier verifié en moins de deux secondes est %d.\n", max);

    return 0;
}
