#include <printf.h>
#include <limits.h>
#include "voting.h"
#include "blockchain.h"
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include "Arborescence.h"


/*
 * ======== TESTS PARTIE 1 ========
 */

long gros_premier() {
    long p = 3;
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

void test_modpow(int nbTests) {
    clock_t start, end;
    double tempsCPU;
    FILE *f = fopen("sortie_pow.txt", "w");

    long b = 12;
    long n = 51;

    long i;
    for (i = 1; i < nbTests + 1; i += 1) {
        printf("%li\n", i);
        long a = 1234 * i;

        start = clock();
        modpow_naive(b, a, n);
        end = clock();
        tempsCPU = ((double) (end - start)) / CLOCKS_PER_SEC;
        fprintf(f, "%ld %f ", a, tempsCPU);

        start = clock();
        modpow(b, a, n);
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

void test_rsa() {
    long p = random_prime_number(3, 7, 5000);
    long q;
    do {
        q = random_prime_number(3, 7, 5000);
    } while (q == p);

    long n, s, u;
    generate_key_values(p, q, &n, &s, &u);
    // Le test de positivité des clés est effectué dans la fonction de génération.

    printf("Clé publique : (%lx, %lx)\n", s, n);
    printf("Clé privée : (%lx, %lx)\n", u, n);

    printf("s * u [n] = %ld\n", (s * u) mod n);

    char message[64] = "Coucou hibou caillou chou";
    size_t len = strlen(message);
    long *encrypted = encrypt(message, s, n);

    printf("Message initilal : %s\n", message);
    printf("Représentation chiffrée :\n");
    printLongVector(encrypted, len);
    printf("\n");

    char *decrypted = decrypt(encrypted, len, u, n);
    printf("Decrypted : %s\n", decrypted);
}

/*
 * ======== TESTS PARTIES 2 ET 3 ========
 */


/*
 * Vérifie les fonctions de passage d'une structure à une chaîne
 * et de retour à la structure fonctionnent correctement.
 */
void tests_str(int nbTests) {
    for (int i = 0; i < nbTests; ++i) {
        Key *pKey = malloc(sizeof(Key));
        Key *sKey = malloc(sizeof(Key));
        init_pair_keys(pKey, sKey, 7, 15);
        char *pRepr = key_to_str(pKey);
        char *sRepr = key_to_str(sKey);
        Key *pKey2 = str_to_key(pRepr);
        Key *sKey2 = str_to_key(sRepr);

        if (!keysEqual(pKey, pKey2) || !keysEqual(sKey, sKey2)) {
            fprintf(stderr, "[test strToKey / keyToStr] ERREUR !");
        }

        Signature *signature = sign(pRepr, sKey);
        char *signRepr = signature_to_str(signature);
        Signature *signature2 = str_to_signature(signRepr);

        if (!signaturesEqual(signature, signature2)) {
            fprintf(stderr, "[test signature_to_str / str_to_signature] ERREUR !");
        }

        Protected *protected = init_protected(pKey, pRepr, signature);
        char *protectedRepr = protected_to_str(protected);
        Protected *protected2 = str_to_protected(protectedRepr);

        if (!protectedEqual(protected, protected2)) {
            fprintf(stderr, "[test protected_to_str / str_to_protected] ERREUR !");
        }

        freeKey(pKey);
        freeKey(pKey2);
        freeKey(sKey);
        freeKey(sKey2);
        free(pRepr);
        free(sRepr);
        freeSignature(signature);
        freeSignature(signature2);
        free(signRepr);
        freeProtected(protected);
        freeProtected(protected2);
        free(protectedRepr);
    }
}

/*
 * Test des opérations de la table de hachage.
 */
void hashTableTest(int nbTests) {
    for (int i = 0; i < nbTests; ++i) {
        __HashTable *hashTable = DEPRECATED_initHashTable(2 * i + 1);
        if (!hashTable) {
            fprintf(stderr, "[hashTableTest / création de la table] ERREUR !");
            continue;
        }
        for (int j = 0; j < 3 * i + 1; ++j) {
            Key *key = malloc(sizeof(Key));
            Key *key2 = malloc(sizeof(Key));
            init_pair_keys(key, key2, 7, 15);
            add(hashTable, j, key);

            if (!DEPRECATED_hashTableContains(hashTable, j)) {
                fprintf(stderr, "[hashTableTest / test1] ERREUR !");
            }

            if (DEPRECATED_hashTableContains(hashTable, j + 1)) {
                fprintf(stderr, "[hashTableTest / test2] ERREUR !");
            }

            if (!keysEqual(key, DEPRECATED_get(hashTable, j))) {
                fprintf(stderr, "[hashTableTest / test3] ERREUR !");
            }

            set(hashTable, j, key2);

            if (!keysEqual(DEPRECATED_get(hashTable, j), key2)) {
                fprintf(stderr, "[hashTableTest / test4] ERREUR !");
            }

            freeKey(key);
            freeKey(key2);
        }
        freeHashTable(hashTable);
    }
}

void test_read() {
    CellKey *voterKeys = read_public_keys("keys.txt");
    printListKeys(voterKeys);
    delete_list_keys(voterKeys);

    printf("\n");

    CellKey *candidateKeys = read_public_keys("candidates.txt");
    printListKeys(candidateKeys);
    delete_list_keys(candidateKeys);

    printf("\n");

    CellProtected *declarations = read_protected();
    print_list_protected(declarations);
    delete_cell_protected(declarations);
}

void testFourni() {
    // Testing Init Keys
    Key *pKey = malloc(sizeof(Key));
    Key *sKey = malloc(sizeof(Key));
    init_pair_keys(pKey, sKey, 3, 7);
    printf("pKey: %lx, %lx\n", pKey->val, pKey->n);
    printf("sKey: %lx, %lx\n", sKey->val, sKey->n);

    // Testing Key Serialization
    char *chaine = key_to_str(pKey);
    printf("key to str: %s \n", chaine);
    Key *k = str_to_key(chaine);
    printf("str to key : %lx , %lx \n", k->val, k->n);

    //Testing signature
    //Candidate keys:
    Key *pKeyC = malloc(sizeof(Key));
    Key *sKeyC = malloc(sizeof(Key));
    init_pair_keys(pKeyC, sKeyC, 3, 7);
    //Declaration:
    char *mess = key_to_str(pKeyC);
    printf("%s vote pour %s\n", key_to_str(pKey), mess);
    Signature *sgn = sign(mess, sKey);
    printf("signature : ");
    printLongVector(sgn->content, sgn->length);
    chaine = signature_to_str(sgn);
    printf(" signature to str : %s \n", chaine);
    sgn = str_to_signature(chaine);
    printf("str to signature : ");
    printLongVector(sgn->content, sgn->length);
    // Testing protected:
    Protected *pr = init_protected(pKey, mess, sgn);
    // Verification:
    if (verify(pr)) {
        printf("Signature valide\n");
    } else {
        printf("Signature non valide\n");
    }
    chaine = protected_to_str(pr);
    printf(" protected to str : %s\n", chaine);
    pr = str_to_protected(chaine);
    printf("str to protected : %s %s %s\n", key_to_str(pr->votersPublicKey), pr->message,
           signature_to_str(pr->signature));
    free(pKey);
    free(sKey);
    free(pKeyC);
    free(sKeyC);
}

void testSSL() {
    const char *s = "Rosetta code";
    unsigned char *d = (unsigned char*) SHA256( (const unsigned char*) s, strlen(s),0);
    int i;
    for (i = 0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x",d[i]);
    }
    putchar('\n');
}

void test_blockchian() {
    generate_random_data(300, 5);
    CellProtected* declarations = read_protected();

    // On rend la première déclaration frauduleuse pour tester la fonction de filtrage anti-fraude.
    declarations->data->message[0]++;
    printf("Liste des déclarations :\n");
    print_list_protected(declarations);
    declarations = delete_liste_fraude(declarations);
    printf("\n\n\nListe des déclarations après filtre anti-fraude :\n");
    print_list_protected(declarations);
    delete_list_protected(declarations);

    /*while (declarations) {
        submit_vote(declarations->data);
        declarations = declarations->next;
    }*/

    /*Key *pKey = malloc(sizeof(Key));
    Key *sKey = malloc(sizeof(Key));
    init_pair_keys(pKey, sKey, 3, 7);
    printf("pKey: %lx, %lx\n", pKey->val, pKey->n);
    printf("sKey: %lx, %lx\n", sKey->val, sKey->n);

    //Declaration:
    char* mess = key_to_str(pKey);
    printf("%s vote pour %s\n",key_to_str(pKey), mess);
    Signature* sgn = sign(mess, sKey);

    Protected *pr = init_protected(pKey,mess,sgn);*/

    /*Block *b = malloc(sizeof(Block));
    b->votes = create_cell_protected(pr);
    compute_proof_of_work(b,4);
    b->author = pKey;
    char* str = blockToStr(b);
    b->hash = strToHash(str);
    b->previous_hash = NULL;*/
}

int main(void) {
    srand(time(NULL));
    printf("\tTest Blockchain.\n");
    test_blockchian();
    printf("\tTerminé.\n");




    /*test_modpow(10000);

    printf("Tests str_to_key et key_to_str.\n");
    tests_str(250);
    printf("\tTerminé.\n");
    printf("Tests __HashTable.\n");
    hashTableTest(25);
    printf("\tTerminé.\n");
    printf("Génération des données de vote.\n");*/
    /*generate_random_data(10, 5);
    printf("\tTerminé.\n");
    printf("Test des fonctions de lecture.\n");
    test_read();
    printf("\tTerminé.\n");*/

    return 0;
}