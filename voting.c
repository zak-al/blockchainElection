#include "voting.h"
#include "HashTable.h"
#include "Protected.h"

/*
 * =========== EXERCICE 4 : GÉNÉRATION DE DONNÉES POUR L'ÉLECTION + LECTURE DES CLÉS ET DÉCLARATIONS ===========
 */

CellProtected *read_protected() {
    CellProtected *list = NULL;

    FILE *file = fopen("declarations.txt", "r");
    char str[256];
    while (fgets(str, 255, file)) {
        Protected *protected = str_to_protected(str);
        list = prependProtected(protected, list);
        freeProtected(protected);
        protected = NULL;
    }

    return list;
}

CellKey *read_public_keys(char *filename) {
    CellKey *list = NULL;

    FILE *file = fopen(filename, "r");
    char str[256];
    while (fgets(str, 255, file)) {
        char public[256];
        sscanf(str, "%[^;]", public);
        Key *key = str_to_key(public);
        list = prependKey(key, list);
        freeKey(key);
    }

    return list;
}

void generate_random_data(int nv, int nc) {
    FILE *keysFile = fopen("keys.txt", "w+");
    FILE *candidatesFile = fopen("candidates.txt", "w+");
    FILE *declarationsFile = fopen("declarations.txt", "w");

    HashTable* candidates = htIkConstruct(nc + 1 + nc / 5);
    HashTable* candidateSecretKeys = htIkConstruct(nc + 1 + nc / 5);

    HashTable* existingKeys = htKiCreateHashTable(NULL, nv + 1 + nv / 5);

    // candidateNumbers est un tableau qui énumère les indices des candidats.
    // Il est utilisé en particulier pour en tirer un au hasard.
    int* candidateNumbers = malloc(nc * sizeof(int));
    if (!candidateNumbers) {
        fprintf(stderr, "[generate_random_data/candidateNumbers] Erreur lors de l'allocation de la mémoire :(\n");
        return;
    }

    // La boucle ci-dessous tire nc candidats distincts, génère leurs clés et les garde en mémoire et les enregistre
    // dans le fichier.
    for (int i = 0; i < nc; ++i) {
        int idx;
        do {
            idx = rand() % nv;
        } while (htIkExists(candidates, idx));

        Key *publicKey = malloc(sizeof(Key));
        Key *secretKey = malloc(sizeof(Key));
        if (!publicKey || !secretKey) {
            fprintf(stderr, "[generate_random_data/publicKey,secretKey] Erreur lors de l'allocation de la mémoire :(\n");
            free(publicKey);
            free(secretKey);
            return;
        }

        // On génère une nouvelle clé qui n'a encore pas été générée.
        do {
            init_pair_keys(publicKey, secretKey, 3, 7);
        } while (htKiExists(existingKeys, publicKey));

        htKiAdd(existingKeys, publicKey, 0);

        char *publicKeyRepr = key_to_str(publicKey);
        char *secretKeyRepr = key_to_str(secretKey);

        htIkAdd(candidates, idx, publicKey);
        htIkAdd(candidateSecretKeys, idx, secretKey);

        fprintf(candidatesFile, "%s\n", publicKeyRepr);

        freeKey(publicKey);
        freeKey(secretKey);
        free(publicKeyRepr);
        free(secretKeyRepr);

        candidateNumbers[i] = idx;
    }

    /*
     * Vote.
     * Pour chaque électeur, on choisit aléatoirement un indice entre 0 et (nc - 1),
     * qui correspond à l'indice d'un candidat.
     * On remplit ainsi le fichier de déclarations.
     */
    for (int i = 0; i < nv; ++i) {
        Key *votersPublicKey;
        Key *votersSecretKey;
        char *candidatesPublicKeyRepr;
        char *protectedRepr;

        printf("DEBUG 1\n");

        printf("%d\n", htIkExists(candidates, i));

        int candidate = FALSE;

        if (htIkExists(candidates, i)) {
            printf("\tDEBUG CAN\n");
            // Ce bloc récupère la clé déjà calculée dans le cas où l'électeur actuel est un candidat.
            votersPublicKey = htIkGetOrNull(candidates, i);
            votersSecretKey = htIkGetOrNull(candidateSecretKeys, i);
            candidate = TRUE;
        } else {
            printf("\tDEBUG NOT CAN\n");
            // Si l'électeur n'est pas candidat, i.e. si sa clé n'a pas déjà été calculée,
            // on la calcule comme on l'a fait pour les candidats.
            votersPublicKey = malloc(sizeof(Key));
            votersSecretKey = malloc(sizeof(Key));

            if (!votersPublicKey || !votersSecretKey) {
                fprintf(stderr, "[generate_random_data/votersPublicKey,votersSecretKey] Erreur lors de l'allocation de la mémoire :(\n");
                freeKey(votersPublicKey);
                freeKey(votersSecretKey);
            }

            do {
                printf("DEBUG hello\n");
                init_pair_keys(votersPublicKey, votersSecretKey, 3, 7);
            } while (htKiExists(existingKeys, votersPublicKey));

            printf("DEBUG Keys chosen\n");

            htKiAdd(existingKeys, votersPublicKey, 0);

            printf("DEBUG Key added\n");
        }

        printf("DEBUG 2\n");

        // Affichage dans le fichier des votants.
        char *publicKeyRepr = key_to_str(votersPublicKey);
        char *secretKeyRepr = key_to_str(votersSecretKey);
        fprintf(keysFile, "%s;%s\n", publicKeyRepr, secretKeyRepr);
        free(publicKeyRepr);
        free(secretKeyRepr);

        printf("DEBUG 3\n");

        // Choix du candidat.
        int candidatesIdx = candidateNumbers[rand() % nc];
        Key *candidatesPublicKey = htIkGetOrNull(candidates, candidatesIdx);

        if (!candidatesPublicKey) {
            fprintf(stderr, "[generate_random_data] Clé du candidat nulle.");
        }

        printf("DEBUG 4\n");

        // Message:
        candidatesPublicKeyRepr = key_to_str(candidatesPublicKey);

        printf("DEBUG 5\n");

        Signature *signature = sign(candidatesPublicKeyRepr, votersSecretKey);
        Protected *protected = init_protected(votersPublicKey, candidatesPublicKeyRepr, signature);
        protectedRepr = protected_to_str(protected);
        fprintf(declarationsFile, "%s\n", protectedRepr);

        printf("DEBUG 6\n");

        free(protectedRepr);
        free(candidatesPublicKeyRepr);
        freeSignature(signature);
        freeProtected(protected);

        if (!candidate) {
            freeKey(votersSecretKey);
            freeKey(votersPublicKey);
        }

        printf("DEBUG 7\n");
    }

    fclose(keysFile);
    fclose(candidatesFile);
    fclose(declarationsFile);

    free(candidateNumbers);
    printf("DEBUG candidatenumbers freed\n");
    deleteHashTable(existingKeys);
    printf("DEBUG existingKeys freed\n");
    deleteHashTable(candidates);
    printf("DEBUG candidates freed\n");
    deleteHashTable(candidateSecretKeys);
    printf("DEBUG candidateSecretKeys freed\n");
}

/*
 * =========== EXERCICE 5 : LISTES DE CLÉS ET DE DÉCLARATIONS ===========
 */

CellKey *create_cell_key(Key *key) {
    CellKey *cellKey = malloc(sizeof(CellKey));
    if (!cellKey) {
        fprintf(stderr, "[create_cell_key] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    cellKey->data = key;
    cellKey->next = NULL;

    return cellKey;
}

CellProtected *create_cell_protected(Protected *pr) {
    CellProtected *cellProtected = malloc(sizeof(CellProtected));
    if (!cellProtected) {
        fprintf(stderr, "[create_cell_protected] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    if (!cellProtected) {
        fprintf(stderr, "[create_cell_protected] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    cellProtected->data = pr;
    cellProtected->next = NULL;

    return cellProtected;
}

/**
 * @brief Crée une nouvelle liste en ajoutant une cellule contenant le clé passée en paramètre en tête de la liste passée.
 * @param key Clé contenue dans la future tête de la liste.
 * @param list Liste à laquelle ajouter la nouvelle cellule.
 * @return Liste mise à jour.
 */
CellKey *prependKey(Key *key, CellKey *list) {
    CellKey *cellKey = create_cell_key(copyKey(key));
    if (!cellKey) {
        fprintf(stderr, "[prependKey] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellKey->next = list;
    return cellKey;
}

CellProtected *prependProtected(Protected *protected, CellProtected *list) {
    CellProtected *cellProtected = create_cell_protected(copyProtected(protected));
    if (!cellProtected) {
        fprintf(stderr, "[prependProtected] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellProtected->next = list;
    return cellProtected;
}

CellProtected* copyCellProtected_shallow(const CellProtected *cellProtected) {
    if (cellProtected == NULL) return NULL;
    CellProtected* copy = malloc(sizeof(CellProtected));
    if (!copy) {
        fprintf(stderr, "[copyCellProtected_shallow/copy] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    CellProtected* current = copy;
    while (cellProtected) {
        current->data = cellProtected->data;
        if (cellProtected->next != NULL) {
            current->next = malloc(sizeof(CellProtected));
            if (!current->next) {
                fprintf(stderr, "[copyCellProtected_shallow/current->next] Erreur lors de l'allocation de la mémoire :(\n");
                delete_cell_protected_shallow(copy);
                return NULL;
            }

            current = current->next;
        } else {
            return copy;
        }
    }

    return copy;
}

void printListKeys(CellKey *list) {
    while (list) {
        char *keyRepr = key_to_str(list->data);
        printf("%s\n", keyRepr);
        free(keyRepr);
        list = list->next;
    }
}

void print_list_protected(CellProtected *list) {
    while (list) {
        char *protectedRepr = protected_to_str(list->data);
        printf("%s \n", protectedRepr);
        free(protectedRepr);
        list = list->next;
    }
}

void delete_cell_key(CellKey *cellKey) {
    if (!cellKey) return;
    freeKey(cellKey->data);
    free(cellKey);
}

void delete_list_keys(CellKey *cellKey) {
    while (cellKey) {
        CellKey *next = cellKey->next;
        delete_cell_key(cellKey);
        cellKey = next;
    }
}

void delete_cell_protected(CellProtected *cellProtected) {
    if (!cellProtected) return;
    freeProtected(cellProtected->data);
    free(cellProtected);
}

void delete_cell_protected_shallow(CellProtected *cellProtected) {
    if (!cellProtected) return;
    free(cellProtected);
}


void delete_list_protected(CellProtected *cellProtected) {
    while (cellProtected) {
        CellProtected *next = cellProtected->next;
        delete_cell_protected(cellProtected);
        cellProtected = next;
    }
}

void delete_list_protected_shallow(CellProtected *cellProtected) {
    while (cellProtected) {
        CellProtected *next = cellProtected->next;
        delete_cell_protected_shallow(cellProtected);
        cellProtected = next;
    }
}

/*
 * =========== EXERCICE 6 ===========
 */

CellProtected *delete_liste_fraude(CellProtected *cellProtected) {
    if (!cellProtected) return NULL;
    while (cellProtected && !verify(cellProtected->data)) {
        CellProtected *next = cellProtected->next;
        delete_cell_protected(cellProtected);
        cellProtected = next;
    }
    CellProtected* orig = cellProtected;

    while (cellProtected && cellProtected->next) {
        CellProtected *tmp  = cellProtected->next;
        if (!verify(tmp->data)) {
            CellProtected *next = tmp->next;
            delete_cell_protected(next);
            cellProtected->next = next;
        }
        cellProtected = cellProtected->next;
    }

    return orig;
}

Key* computeWinner(CellProtected* declarations, CellKey* candidates, CellKey* voters, size_t sizeC, size_t sizeV) {
    /*
     * On fait l'hypothèse que les signatures déclarations sont toutes valides.
     */
    HashTable* candidatesHashTable = htKiCreateHashTable(candidates, sizeC);
    HashTable* votersHashTable = htKiCreateHashTable(voters, sizeV);

    while (declarations) {
        Key* votersKey = declarations->data->votersPublicKey;
        int* numberOfVotesFromVoter = htKiGetOrNull(votersHashTable, votersKey);
        if (numberOfVotesFromVoter == NULL || *numberOfVotesFromVoter != 0) {
            /*
             * Erreur : l'électeur n'a pas le droit de vote ou a déjà voté.
             */
            free(numberOfVotesFromVoter);
            continue;
        }

        char* message = declarations->data->message;
        Key* candidatesKey = str_to_key(message);
        int* numberOfVotesForCandidate = htKiGetOrNull(candidatesHashTable, candidatesKey);

        /*
         * On vérifie que le candidat soit bien enregistré en tant que candidat.
         * Si c'est le cas, on augmente son score de 1.
         */
        if (htKiGetOrNull(candidatesHashTable, candidatesKey) != NULL) {
            htKiIncrement(candidatesHashTable, candidatesKey);
        }

        free(numberOfVotesForCandidate);
        free(numberOfVotesFromVoter);
        free(candidatesKey);
        declarations = declarations->next;
    }

    deleteHashTable(candidatesHashTable);
    deleteHashTable(votersHashTable);

    Key* winnerKey = htKiArgmax(votersHashTable);

    return winnerKey;
}

