#include "voting.h"
#include "HashTable.h"
#include "Protected.h"

/*
 * =========== EXERCICE 4 : GÉNÉRATION DE DONNÉES POUR L'ÉLECTION + LECTURE DES CLÉS ET DÉCLARATIONS ===========
 */

CellProtected* readProtected(char* filename) {
    CellProtected* list = NULL;

    FILE* file = fopen(filename, "r");
    char str[256];
    while (fgets(str, 255, file)) {
        Protected* protected = strToProtected(str);
        list = prependProtected(protected, list);
        freeProtected(protected);
        protected = NULL;
    }

    return list;
}

CellKey* readPublicKeys(char* filename) {
    CellKey* list = NULL;

    FILE* file = fopen(filename, "r");
    char str[256];
    while (fgets(str, 255, file)) {
        char public[256];
        sscanf(str, "%[^;]", public);
        Key* key = strToKey(public);
        list = prependKey(key, list);
        freeKey(key);
    }

    return list;
}

void generateRandomData(int nv, int nc) {
    FILE* keysFile = fopen("keys.txt", "w+");
    FILE* candidatesFile = fopen("candidates.txt", "w+");
    FILE* declarationsFile = fopen("declarations.txt", "w");

    HashTable* candidates = htIkConstruct(nc + 1 + nc / 5);
    HashTable* candidateSecretKeys = htIkConstruct(nc + 1 + nc / 5);

    HashTable* existingKeys = htKiCreateHashTable(NULL, nv + 1 + nv / 5);

    // candidateNumbers est un tableau qui énumère les indices des candidats.
    // Il est utilisé en particulier pour en tirer un au hasard.
    int* candidateNumbers = malloc(nc * sizeof(int));
    if (!candidateNumbers) {
        fprintf(stderr, "[generateRandomData/candidateNumbers] Erreur lors de l'allocation de la mémoire :(\n");
        return;
    }

    // La boucle ci-dessous tire nc candidats distincts, génère leurs clés et les garde en mémoire et les enregistre
    // dans le fichier.
    for (int i = 0; i < nc; ++i) {
        int idx;
        do {
            idx = rand() % nv;
        } while (htIkExists(candidates, idx));

        Key* publicKey = malloc(sizeof(Key));
        Key* secretKey = malloc(sizeof(Key));
        if (!publicKey || !secretKey) {
            fprintf(stderr,
                    "[generateRandomData/publicKey,secretKey] Erreur lors de l'allocation de la mémoire :(\n");
            free(publicKey);
            free(secretKey);
            return;
        }

        // On génère une nouvelle clé qui n'a encore pas été générée.
        do {
            initPairKeys(publicKey, secretKey, 3, 7);
        } while (htKiExists(existingKeys, publicKey));

        htKiAdd(existingKeys, publicKey, 0);

        char* publicKeyRepr = keyToStr(publicKey);
        char* secretKeyRepr = keyToStr(secretKey);

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
        Key* votersPublicKey;
        Key* votersSecretKey;
        char* candidatesPublicKeyRepr;
        char* protectedRepr;

        int candidate = FALSE;

        if (htIkExists(candidates, i)) {
            // Ce bloc récupère la clé déjà calculée dans le cas où l'électeur actuel est un candidat.
            votersPublicKey = htIkGetOrNull(candidates, i);
            votersSecretKey = htIkGetOrNull(candidateSecretKeys, i);
            candidate = TRUE;
        } else {
            // Si l'électeur n'est pas candidat, i.e. si sa clé n'a pas déjà été calculée,
            // on la calcule comme on l'a fait pour les candidats.
            votersPublicKey = malloc(sizeof(Key));
            votersSecretKey = malloc(sizeof(Key));

            if (!votersPublicKey || !votersSecretKey) {
                fprintf(stderr,
                        "[generateRandomData/votersPublicKey,votersSecretKey] Erreur lors de l'allocation de la mémoire :(\n");
                freeKey(votersPublicKey);
                freeKey(votersSecretKey);
            }

            do {
                initPairKeys(votersPublicKey, votersSecretKey, 3, 7);
            } while (htKiExists(existingKeys, votersPublicKey));

            htKiAdd(existingKeys, votersPublicKey, 0);
        }

        // Affichage dans le fichier des votants.
        char* publicKeyRepr = keyToStr(votersPublicKey);
        char* secretKeyRepr = keyToStr(votersSecretKey);
        fprintf(keysFile, "%s;%s\n", publicKeyRepr, secretKeyRepr);
        free(publicKeyRepr);
        free(secretKeyRepr);

        // Choix du candidat.
        int candidatesIdx = candidateNumbers[rand() % nc];
        Key* candidatesPublicKey = htIkGetOrNull(candidates, candidatesIdx);

        if (!candidatesPublicKey) {
            fprintf(stderr, "[generateRandomData] Clé du candidat nulle.");
        }

        // Message:
        candidatesPublicKeyRepr = keyToStr(candidatesPublicKey);

        Signature* signature = sign(candidatesPublicKeyRepr, votersSecretKey);
        Protected* protected = initProtected(votersPublicKey, candidatesPublicKeyRepr, signature);
        protectedRepr = protectedToStr(protected);
        fprintf(declarationsFile, "%s\n", protectedRepr);

        free(protectedRepr);
        free(candidatesPublicKeyRepr);
        freeSignature(signature);
        freeProtected(protected);

        if (!candidate) {
            freeKey(votersSecretKey);
            freeKey(votersPublicKey);
        }
    }

    fclose(keysFile);
    fclose(candidatesFile);
    fclose(declarationsFile);

    free(candidateNumbers);
    deleteHashTable(existingKeys);
    deleteHashTable(candidates);
    deleteHashTable(candidateSecretKeys);
}

/*
 * =========== EXERCICE 5 : LISTES DE CLÉS ET DE DÉCLARATIONS ===========
 */

CellKey* createCellKey(Key* key) {
    CellKey* cellKey = malloc(sizeof(CellKey));
    if (!cellKey) {
        fprintf(stderr, "[createCellKey] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    cellKey->data = key;
    cellKey->next = NULL;

    return cellKey;
}

CellProtected* createCellProtected(Protected* pr) {
    CellProtected* cellProtected = malloc(sizeof(CellProtected));
    if (!cellProtected) {
        fprintf(stderr, "[createCellProtected] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    if (!cellProtected) {
        fprintf(stderr, "[createCellProtected] Erreur lors de l'allocation de la mémoire :(\n");
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
CellKey* prependKey(Key* key, CellKey* list) {
    CellKey* cellKey = createCellKey(copyKey(key));
    if (!cellKey) {
        fprintf(stderr, "[prependKey] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellKey->next = list;
    return cellKey;
}

CellProtected* prependProtected(Protected* protected, CellProtected* list) {
    CellProtected* cellProtected = createCellProtected(copyProtected(protected));
    if (!cellProtected) {
        fprintf(stderr, "[prependProtected] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellProtected->next = list;
    return cellProtected;
}

CellProtected* copyCellProtectedShallow(const CellProtected* cellProtected) {
    if (cellProtected == NULL) return NULL;
    CellProtected* copy = malloc(sizeof(CellProtected));
    if (!copy) {
        fprintf(stderr, "[copyCellProtectedShallow/copy] Erreur lors de l'allocation de la mémoire :(\n");
        return NULL;
    }

    CellProtected* current = copy;
    while (cellProtected) {
        current->data = cellProtected->data;
        if (cellProtected->next != NULL) {
            current->next = malloc(sizeof(CellProtected));
            if (!current->next) {
                fprintf(stderr,
                        "[copyCellProtectedShallow/current->next] Erreur lors de l'allocation de la mémoire :(\n");
                deleteCellProtectedShallow(copy);
                return NULL;
            }

            current = current->next;
        } else {
            return copy;
        }
    }

    return copy;
}

void printListKeys(CellKey* list) {
    while (list) {
        char* keyRepr = keyToStr(list->data);
        printf("%s\n", keyRepr);
        free(keyRepr);
        list = list->next;
    }
}

void printListProtected(CellProtected* list) {
    while (list) {
        char* protectedRepr = protectedToStr(list->data);
        printf("%s \n", protectedRepr);
        free(protectedRepr);
        list = list->next;
    }
}

void deleteCellKey(CellKey* cellKey) {
    if (!cellKey) return;
    freeKey(cellKey->data);
    free(cellKey);
}

void deleteListKeys(CellKey* cellKey) {
    while (cellKey) {
        CellKey* next = cellKey->next;
        deleteCellKey(cellKey);
        cellKey = next;
    }
}

int cellProtectedEqual(const CellProtected* one, const CellProtected* two) {
    while (one && two) {
        if (!protectedEqual(one->data, two->data)) {
            return FALSE;
        } else {
            one = one->next;
            two = two->next;
        }
    }

    if (one || two) {
        return FALSE;
    }

    return TRUE;
}

CellProtected* reverseCellProteted(CellProtected* list) {
    CellProtected* res = NULL;
    while (list) {
        CellProtected* next = list->next;
        list->next = res;
        res = list;
        list = next;
    }

    return res;
}

void deleteCellProtected(CellProtected* cellProtected) {
    if (!cellProtected) {
        return;
    }

    freeProtected(cellProtected->data);
    free(cellProtected);
}

void deleteCellProtectedShallow(CellProtected* cellProtected) {
    if (!cellProtected) return;
    free(cellProtected);
}


void deleteListProtected(CellProtected* cellProtected) {
    while (cellProtected) {
        CellProtected* next = cellProtected->next;
        deleteCellProtected(cellProtected);
        cellProtected = next;
    }
}

void deleteListProtectedShallow(CellProtected* cellProtected) {
    while (cellProtected) {
        CellProtected* next = cellProtected->next;
        deleteCellProtectedShallow(cellProtected);
        cellProtected = next;
    }
}

// QUESTION 8.8
CellProtected* merge(CellProtected* l1, CellProtected* l2) {
    if (!l1) return l2;
    while (l1->next) {
        l1 = l1->next;
    }
    l1->next = l2;
    return l1;
}

CellProtected* _merge(CellProtected* cp1, CellProtected* cp2) {
    while (cp2) {
        prependProtected(cp2->data, cp1);
        cp2 = cp2->next;
    }
    return cp1;
}

/*
 * =========== EXERCICE 6 ===========
 */

CellProtected* deleteListeFraude(CellProtected* cellProtected) {
    if (!cellProtected) return NULL;
    while (cellProtected && !verify(cellProtected->data)) {
        CellProtected* next = cellProtected->next;
        deleteCellProtected(cellProtected);
        cellProtected = next;
    }
    CellProtected* orig = cellProtected;

    while (cellProtected && cellProtected->next) {
        CellProtected* tmp = cellProtected->next;
        if (!verify(tmp->data)) {
            CellProtected* next = tmp->next;
            deleteCellProtected(next);
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

        Key* candidatesKey = strToKey(message);
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

    Key* winnerKey = htKiArgmax(candidatesHashTable);
    deleteHashTable(candidatesHashTable);
    deleteHashTable(votersHashTable);

    return winnerKey;
}
