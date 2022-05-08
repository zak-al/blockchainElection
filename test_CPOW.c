#include <openssl/sha.h>
#include "blockchain.h"
#include <time.h>

#define NB_TEST 5

int main(){
    generate_random_data(10,3);
    CellProtected * votes = read_protected();
    CellKey* voters = read_public_keys("keys.txt");
    printf("voters initialised\n");
    Key* author = voters->data;
    Key* author1 = voters->next->next;

    Block *b1 = malloc(sizeof(Block));
    b1->author = author1;
    b1->previous_hash = NULL;
    b1->votes = votes;
    b1->nonce = 0;
    b1->hash = (unsigned char *) strToHash(blockToStr(b1));

    Block *b = malloc(sizeof(Block));
    b->author = author;
    b->previous_hash = strdup(b1->hash);
    b->votes = votes;
    b->nonce = 0;
    b->hash = (unsigned char *) strToHash(blockToStr(b));

    //Question 1.5
    //pour comparer les performances des deux methodes
    //on socker la vitesse des deux fonctions avec les meme parametres

    clock_t temps_init, temps_fin;
    double temps_cpu;
    FILE * fic = fopen("sortieTime_CPOW.txt", "w");  //stocker les donnees dans le ficher
    if (fic == NULL) {
        printf("Erreur de fichier\n");
        return 0;
    }
    int i = 1;
    while (i < 4) {
        temps_init = clock();
        compute_proof_of_work(b, i);
        temps_fin = clock();
        temps_cpu = ((double) (temps_fin - temps_init) / CLOCKS_PER_SEC);
        fprintf(fic, "%d %f\n", i, temps_cpu);
        if (temps_cpu >= 1)
            printf("%d\n", i);
        i++;
    }
    fclose(fic);

    freeBlock(b);
    free(author);
    return 0;
}