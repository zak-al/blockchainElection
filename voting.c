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

/**
 * Prend un nombre nv d'électeurs et un nombre nc de candidats et génère les fichiers keys.txt, candidats.txt et declarations.txt
 * contenant respectivement les clés publiques et privées de tous les électeurs, les clés publiques de tous les candidats
 * et les votes, tirés aléatoirement, de tous les électeurs.
 * Il est garanti que tous les couples (clé publique, clé privée) sont distincts
 * et qu'il y a exactement nv électeurs et nc candidats distincts
 * @param nv
 * @param nc
 */
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

/**
 * Ajoute protected en tête de list. list n'est pas modifiée, la nouvelle liste est renvoyée.
 * Si list est vide (nulle) alors protected est renvoyé tel quel.
 * @param protected
 * @param list
 * @return
 */
CellProtected* prependProtected(Protected* protected, CellProtected* list) {
    CellProtected* cellProtected = createCellProtected(copyProtected(protected));
    if (!cellProtected) {
        fprintf(stderr, "[prependProtected] Erreur lors de l'allocation de la mémoire de la nouvelle cellule :(\n");
        return list;
    }

    cellProtected->next = list;
    return cellProtected;
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

/**
 * Supprime cellKey et la clé qu'elle contient.
 * @param cellKey
 */
void deleteCellKey(CellKey* cellKey) {
    if (!cellKey) return;
    freeKey(cellKey->data);
    free(cellKey);
}

/**
 * Supprime une liste de clés dans son entièreté : structure comme contenu.
 * @param cellKey
 */
void deleteListKeys(CellKey* cellKey) {
    while (cellKey) {
        CellKey* next = cellKey->next;
        deleteCellKey(cellKey);
        cellKey = next;
    }
}

/**
 * Teste l'égalité entre deux listes de cellules. Deux listes de cellules sont dites égales si les pointeurs
 * qui les représentent sont égaux ou si elles contiennent exactement les mêmes déclarations, dans le même ordre.
 * @param one
 * @param two
 * @return
 */
int cellProtectedEqual(const CellProtected* one, const CellProtected* two) {
    if (one == two) return TRUE;
    if (!one || !two) return FALSE;

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

/**
 * Suppression de la structure d'une cellule de déclaration : le contenu n'est pas supprimé.
 * @param cellProtected
 */
void deleteCellProtectedShallow(CellProtected* cellProtected) {
    if (!cellProtected) return;
    free(cellProtected);
}

/**
 * Supprime une liste de déclarations intégralement : structure et contenu.
 * @param cellProtected
 */
void deleteListProtected(CellProtected* cellProtected) {
    while (cellProtected) {
        CellProtected* next = cellProtected->next;
        deleteCellProtected(cellProtected);
        cellProtected = next;
    }
}

/**
 * Supprime la structure d'une liste de déclarations.
 * @param cellProtected
 */
void deleteListProtectedShallow(CellProtected* cellProtected) {
    while (cellProtected) {
        CellProtected* next = cellProtected->next;
        deleteCellProtectedShallow(cellProtected);
        cellProtected = next;
    }
}

// QUESTION 8.8
/**
 * Fusionne l1 et l2. l1 se retrouve avant l2. Les paramètres sont modifiés, et un pointeur vers la liste créée est renvoyé.
 * Le pointeur renvoyé est égal au pointeur l1 si l1 n'était pas nul. Si l1 est nul, le pointeur renvoyé est égal à l2.
 * @param l1
 * @param l2
 * @return
 */
CellProtected* merge(CellProtected* l1, CellProtected* l2) {
    if (!l1) return l2;
    while (l1->next) {
        l1 = l1->next;
    }
    l1->next = l2;
    return l1;
}

/*
 * =========== EXERCICE 6 ===========
 */

/**
 * Renvoie une nouvelle liste de déclarations après suppression des déclarations frauduleuses.
 * Le paramètre est modifié.
 * @param cellProtected
 * @return
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

/**
 * Renvoie le candidat ayant le plus de vois parmi les votes contenus dans declarations.
 * Vérifie que les électeurs ne votent qu'une seule fois.
 * NE VÉRIFIE PAS que les déclarations ne sont pas frauduleuses.
 * @param declarations
 * @param candidates
 * @param voters
 * @param sizeC un entier un peu plus grand que le nombre de candidats, utilisé comme taille d'une table de hachage de candidats avec probing
 * @param sizeV un entier un peu plus grand que le nombre d'électeurs, utilisé comme taille d'une table de hachage d'électeurs avec probing
 * @return
 */
Key* computeWinner(CellProtected* declarations, CellKey* candidates, CellKey* voters, size_t sizeC, size_t sizeV) {
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
