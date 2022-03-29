#include <printf.h>
#include "voting.h"

/*
 * Vérifie les fonctions de passage d'une structure à une chaîne
 * et de retour à la structure fonctionnent correctement.
 */
void tests_str(int nbTests) {
    for (int i = 0; i < nbTests; ++i) {
        Key* pKey = malloc(sizeof(Key));
        Key* sKey = malloc(sizeof(Key));
        init_pair_keys(pKey, sKey, 7, 15);
        char* pRepr = key_to_str(pKey);
        char* sRepr = key_to_str(sKey);
        Key* pKey2 = str_to_key(pRepr);
        Key* sKey2 = str_to_key(sRepr);

        if (!keysEqual(pKey, pKey2) || !keysEqual(sKey, sKey2)) {
            fprintf(stderr, "[test strToKey / keyToStr] ERREUR !");
        }

        Signature* signature = sign(pRepr, sKey);
        char* signRepr = signature_to_str(signature);
        Signature* signature2 = str_to_signature(signRepr);

        if (!signaturesEqual(signature, signature2)) {
            fprintf(stderr, "[test signature_to_str / str_to_signature] ERREUR !");
        }

        Protected* protected = init_protected(pKey, pRepr, signature);
        char* protectedRepr = protected_to_str(protected);
        Protected* protected2 = str_to_protected(protectedRepr);

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

void hashTableTest(int nbTests) {
    for (int i = 0; i < nbTests; ++i) {
        HashTable* hashTable = initHashTable(2 * i + 1);
        if (!hashTable) {
            fprintf(stderr, "[hashTableTest / création de la table] ERREUR !");
            continue;
        }
        for (int j = 0; j < 3 * i + 1; ++j) {
            Key* key = malloc(sizeof(Key));
            Key* key2 = malloc(sizeof(Key));
            init_pair_keys(key, key2, 7, 15);
            add(hashTable, j, key);

            if (!hashTableContains(hashTable, j)) {
                fprintf(stderr, "[hashTableTest / test1] ERREUR !");
            }

            if (hashTableContains(hashTable, j + 1)) {
                fprintf(stderr, "[hashTableTest / test2] ERREUR !");
            }

            if (!keysEqual(key, get(hashTable, j))) {
                fprintf(stderr, "[hashTableTest / test3] ERREUR !");
            }

            set(hashTable, j, key2);

            if (!keysEqual(get(hashTable, j), key2)) {
                fprintf(stderr, "[hashTableTest / test4] ERREUR !");
            }

            freeKey(key);
            freeKey(key2);
        }
        freeHashTable(hashTable);
    }
}

int main(void) {
    srand(time(NULL));

    printf("Tests str_to_key et key_to_str.\n");
    tests_str(250);
    printf("Terminé.\n");
    printf("Tests HashTable.\n");
    hashTableTest(25);
    printf("Terminé.\n");
    generate_random_data(1000, 12);

    return 0;
}