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
        DEPRECATED__HashTable *hashTable = DEPRECATED_initHashTable(2 * i + 1);
        if (!hashTable) {
            fprintf(stderr, "[hashTableTest / création de la table] ERREUR !");
            continue;
        }
        for (int j = 0; j < 3 * i + 1; ++j) {
            Key *key = malloc(sizeof(Key));
            Key *key2 = malloc(sizeof(Key));
            init_pair_keys(key, key2, 7, 15);
            DEPRECATED_add(hashTable, j, key);

            if (!DEPRECATED_hashTableContains(hashTable, j)) {
                fprintf(stderr, "[hashTableTest / test1] ERREUR !");
            }

            if (DEPRECATED_hashTableContains(hashTable, j + 1)) {
                fprintf(stderr, "[hashTableTest / test2] ERREUR !");
            }

            if (!keysEqual(key, DEPRECATED_get(hashTable, j))) {
                fprintf(stderr, "[hashTableTest / test3] ERREUR !");
            }

            DEPRECATED_set(hashTable, j, key2);

            if (!keysEqual(DEPRECATED_get(hashTable, j), key2)) {
                fprintf(stderr, "[hashTableTest / test4] ERREUR !");
            }

            freeKey(key);
            freeKey(key2);
        }
        DEPRECATED_freeHashTable(hashTable);
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
    unsigned char d[SHA256_DIGEST_LENGTH];
    SHA256((const unsigned char*) s, strlen(s),d);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++){
        printf("%02x", d[i]);
    }
    putchar('\n');
}

void testArborescence() {

}

/**
 * @brief Termine la liste passée en paramètre à la position i
 * et renvoit un pointeur vers la liste commençant à l'élément à la position i + 1.
 * On suppose que la liste passée en paramètre ne se termine pas avant la position i.
 */
CellProtected* splitList(CellProtected* list, int i) {
    while (i > 1) {
        --i;
        list = list->next;
    }
    CellProtected* res = list->next;
    list->next = NULL;
    return res;
}

void testWriteBlockReadBlock() {
    int d = 1;

    printf("hibou\n");
    generate_random_data(10, 3);
    printf("test\n");
    CellProtected* declarations = read_protected();
    CellProtected* declarations2 = splitList(declarations, 5);

    print_list_protected(declarations);
    printf("\n\n");
    print_list_protected(declarations2);
    printf("\n\n");

    printf("declarations initialised\n");
    CellKey* voters = read_public_keys("keys.txt");
    printf("voters initialised\n");
    Key* author1 = voters->data;
    Key* author2 = voters->next->data;

    Block* block1 = malloc(sizeof(Block));
    Block* block1verif;
    Block* block2 = malloc(sizeof(Block));
    Block* block2verif;

    printf("blocks allocated\n");

    block1->hash = NULL;
    block1->previous_hash = NULL;
    block1->author = author1;
    block1->votes = declarations;

    printf("TEST Computing proof of work\n");
    compute_proof_of_work(block1, d);
    printf("\tdone\n");

    printf("TEST Verifying block1\n");
    printf("\t%s\n", verify_block(block1, d) ? "OK" : "ERROR");

    printf("TEST Writing block1\n");
    writeBlock("block1.txt", block1);
    printf("\tdone\n");

    printf("TEST Reading block1 into block1verif\n");
    block1verif = readBlock("block1.txt");
    printf("\tdone\n");

    printf("block1.hash = ");
    DEBUG_printHashHex(block1->hash);
    printf("\n");

    printf("block1 = block1verif ? %s\n", blocksEqual(block1, block1verif) ? "TRUE" : "FALSE");
    writeBlock("block1verif.txt", block1verif);

    printf("Votes in block1:\n");
    print_list_protected(block1->votes);
    printf("\n");

    block2->hash = NULL;
    block2->previous_hash = (unsigned char*) strdup((const char*) block1->hash);
    block2->author = author2;
    block2->votes = declarations2;

    compute_proof_of_work(block2, d);

    printf("block2.hash = ");
    DEBUG_printHashHex(block2->hash);
    printf("\n");

    printf("TEST Verifying block2\n");
    printf("\t%s\n", verify_block(block2, d) ? "OK" : "ERROR");
    printf("TEST Writing block2\n");
    writeBlock("block2.txt", block2);
    printf("\tdone\n");
    printf("Votes in block2:\n");
    print_list_protected(block2->votes);
    printf("\n");

    printf("TEST Reading block2 into block2verif\n");
    block2verif = readBlock("block2.txt");
    printf("\tdone\n");

    printf("block2 = block2verif ? %s\n", blocksEqual(block2, block2verif) ? "TRUE" : "FALSE");
    printf("block2.previousHash = block1.hash ? %s\n", strcmp((const char*) block2->previous_hash, (const char*) block1->hash) == 0 ? "TRUE" : "FALSE");
    writeBlock("block2verif.txt", block2verif);

    freeBlockShallow(block1);
    printf("block1 freed\n");
    freeBlock(block1verif);
    printf("block1verif freed\n");
    freeBlock(block2);
    printf("block2 freed\n");
    freeBlockShallow(block2verif);
    printf("blockVerif freed\n");
    delete_list_keys(voters);
    printf("voters freed\n");
    delete_list_protected(declarations);
    printf("declarations freed\n");
}

void test_blockchian() {
    generate_random_data(300, 5);
    CellProtected* declarations = read_protected();

    // On rend la première déclaration frauduleuse pour tester la fonction de filtrage anti-fraude.
    declarations->data->message[0]++;

    //printf("Liste des déclarations :\n");
    //print_list_protected(declarations);
    //declarations = delete_liste_fraude(declarations);
    //printf("\n\n\nListe des déclarations après filtre anti-fraude :\n");
    //print_list_protected(declarations);
    //delete_list_protected(declarations);

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

    //generate_random_data(10, 3);

    testWriteBlockReadBlock();

    return 0;
    printf("\tTest Blockchain.\n");
    test_blockchian();
    printf("\tTerminé.\n");




    /*test_modpow(10000);

    printf("Tests str_to_key et key_to_str.\n");
    tests_str(250);
    printf("\tTerminé.\n");
    printf("Tests DEPRECATED__HashTable.\n");
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