#include <printf.h>
#include "voting.h"
#include "blockchain.h"
#include <openssl/sha.h>
#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include "Arborescence.h"
#include "Simulation.h"
#include "Constants.h"
#include "Hash.h"

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
        res = isPrimeNaive(p);
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
        modpowNaive(b, a, n);
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
        if (isPrimeMiller(p, 1)) {
            f++;
        }
    }
    fin = clock();
    printf("%d faux positifs\n", f);
    tps = ((double) (fin - deb)) / CLOCKS_PER_SEC;
    printf("en %f secondes.\n", tps);
}

void test_rsa() {
    long p = randomPrimeNumber(3, 7, 5000);
    long q;
    do {
        q = randomPrimeNumber(3, 7, 5000);
    } while (q == p);

    long n, s, u;
    generateKeyValues(p, q, &n, &s, &u);
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
 * Vérifie les fonctions de passage d'une structure à une chaîne
 * et de retour à la structure fonctionnent correctement.
 */
void tests_str(int nbTests) {
    for (int i = 0; i < nbTests; ++i) {
        Key *pKey = malloc(sizeof(Key));
        Key *sKey = malloc(sizeof(Key));
        initPairKeys(pKey, sKey, 7, 15);
        char *pRepr = keyToStr(pKey);
        char *sRepr = keyToStr(sKey);
        Key *pKey2 = strToKey(pRepr);
        Key *sKey2 = strToKey(sRepr);

        if (!keysEqual(pKey, pKey2) || !keysEqual(sKey, sKey2)) {
            fprintf(stderr, "[test strToKey / keyToStr] ERREUR !");
        }

        Signature *signature = sign(pRepr, sKey);
        char *signRepr = signature_to_str(signature);
        Signature *signature2 = str_to_signature(signRepr);

        if (!signaturesEqual(signature, signature2)) {
            fprintf(stderr, "[test signature_to_str / str_to_signature] ERREUR !");
        }

        Protected *protected = initProtected(pKey, pRepr, signature);
        char *protectedRepr = protectedToStr(protected);
        Protected *protected2 = strToProtected(protectedRepr);

        if (!protectedEqual(protected, protected2)) {
            fprintf(stderr, "[test protectedToStr / strToProtected] ERREUR !");
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

void test_read() {
    CellKey *voterKeys = readPublicKeys("keys.txt");
    printListKeys(voterKeys);
    deleteListKeys(voterKeys);

    printf("\n");

    CellKey *candidateKeys = readPublicKeys("candidates.txt");
    printListKeys(candidateKeys);
    deleteListKeys(candidateKeys);

    printf("\n");

    CellProtected *declarations = readProtected("declarations.txt");
    printListProtected(declarations);
    deleteCellProtected(declarations);
}

void testsEX3() {
    // Testing Init Keys
    Key *pKey = malloc(sizeof(Key));
    Key *sKey = malloc(sizeof(Key));
    initPairKeys(pKey, sKey, 3, 7);
    printf("pKey: %lx, %lx\n", pKey->val, pKey->n);
    printf("sKey: %lx, %lx\n", sKey->val, sKey->n);

    // Testing Key Serialization
    char *chaine = keyToStr(pKey);
    printf("key to str: %s \n", chaine);
    Key *k = strToKey(chaine);
    printf("str to key : %lx , %lx \n", k->val, k->n);

    //Testing signature
    //Candidate keys:
    Key *pKeyC = malloc(sizeof(Key));
    Key *sKeyC = malloc(sizeof(Key));
    initPairKeys(pKeyC, sKeyC, 3, 7);
    //Declaration:
    char *mess = keyToStr(pKeyC);
    printf("%s vote pour %s\n", keyToStr(pKey), mess);
    Signature *sgn = sign(mess, sKey);
    printf("signature : ");
    printLongVector(sgn->content, sgn->length);
    chaine = signature_to_str(sgn);
    printf(" signature to str : %s \n", chaine);
    sgn = str_to_signature(chaine);
    printf("str to signature : ");
    printLongVector(sgn->content, sgn->length);
    // Testing protected:
    Protected *pr = initProtected(pKey, mess, sgn);
    // Verification:
    if (verify(pr)) {
        printf("Signature valide\n");
    } else {
        printf("Signature non valide\n");
    }
    chaine = protectedToStr(pr);
    printf(" protected to str : %s\n", chaine);
    pr = strToProtected(chaine);
    printf("str to protected : %s %s %s\n", keyToStr(pr->votersPublicKey), pr->message,
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
    printf("================ Test de la génération des données aléatoires et des blocs : écriture et lecture, initialisation, libération ================\n");

    int d = 2;

    printf("Génération et traitement des données de test.\n");
    generateRandomData(10, 3);

    CellProtected* declarations = readProtected("declarations.txt");
    CellProtected* declarations2 = splitList(declarations, 5);

    CellKey* voters = readPublicKeys("keys.txt");

    Key* author1 = voters->data;
    Key* author2 = voters->next->data;
    printf("\tOK\n");

    Block* block1 = malloc(sizeof(Block));
    Block* block1verif;
    Block* block2 = malloc(sizeof(Block));
    Block* block2verif;

    printf("\nTest du premier bloc.\n");

    block1->hash = NULL;
    block1->previous_hash = NULL;
    block1->author = author1;
    block1->votes = declarations;

    printf("\tCalcul de la preuve de travail.\n");
    computeProofOfWork(block1, d);
    printf("\t\tTerminé.\n");

    printf("\tTest de la validité de la preuve de travail.\n");
    printf("\t\t%s\n", verify_block(block1, d) ? "Ok." : "Erreur.");

    printf("\tÉcriture du premier bloc dans un fichier.\n");
    writeBlock("block1.txt", block1);
    printf("\t\tTerminé.\n");

    printf("\tLecture du fichier dans un nouveau bloc.\n");
    block1verif = readBlock("block1.txt");
    printf("\t\tTerminé.\n");

    printf("\tHash : ");

    printf("\tTest de l'égalité entre tous les champs du premier bloc et du bloc lu : ");
    printf("block1 = block1verif ? %s\n", blocksEqual(block1, block1verif) ? "égaux" : "différents.");

    printf("\tÉcriture du bloc lu dans un fichier.\n");
    writeBlock("block1verif.txt", block1verif);
    printf("\t\tTerminé.\n");

    printf("\nTest du deuxième bloc.\n");

    block2->hash = NULL;
    block2->previous_hash = duplicateHash(block1->hash);
    block2->author = author2;
    block2->votes = declarations2;

    printf("\tCalcul de la preuve de travail.\n");
    computeProofOfWork(block2, d);
    printf("\t\tTerminé.\n");

    printf("\tHash : ");

    printf("\tTest de la validité de la preuve de travail.\n");
    printf("\t\t%s\n", verify_block(block2, d) ? "Ok." : "Erreur.");

    printf("\tÉcriture du deuxième bloc dans un fichier.\n");
    writeBlock("block2.txt", block2);
    printf("\t\tTerminé.\n");

    printf("\tLecture du fichier dans un nouveau bloc.\n");
    block2verif = readBlock("block2.txt");
    printf("\t\tTerminé.\n");

    printf("\tTest de l'égalité entre tous les champs du deuxième bloc et du bloc lu : ");
    printf("block2 = block2verif ? %s\n", blocksEqual(block2, block2verif) ? "égaux" : "différents.");

    printf("\tVérification que le champ previousHash du deuxième bloc est le champ hash du premier bloc.\n");
    printf("block2.previousHash = block1.hash ? %s\n", hashEqual(block2->previous_hash, block1->hash) ? "Ok." : "Erreur.");

    printf("\tÉcriture du bloc lu dans un nouveau fichier.\n");
    writeBlock("block2verif.txt", block2verif);
    printf("\t\tTerminé.\n");


    printf("\nTest du troisième bloc.\n");

    Block* block3 = malloc(sizeof(Block));
    block3->author = voters->next->next->data;
    block3->previous_hash = duplicateHash(block1->hash);

    printf("\nVérification que block3.previousHash != block2.hash :\n");
    printf("\tblock3.previousHash != block2.hash ? %s\n", hashEqual(block3->previous_hash, block2->hash) ? "Erreur." : "Ok.");

    printf("\nFin du test d'initialisation, de lecture et d'écriture des blocs.\n");
    printf("Libération de la mémoire.\n");

    freeBlockShallow(block1);
    freeBlock(block1verif);
    freeBlock(block2);
    freeBlock(block2verif);
    deleteListKeys(voters);
    deleteListProtected(declarations);
    printf("\tTerminé.\n");

}

void test_voteSimulation() {
    int d = 1;
    int nv = 1000;
    int nc = 5;
    int sizeC = nc + nc / 12 + 3;
    int sizeV = nv + nv / 3 + 1;

    printf("================ Simulation d'un processus de vote ================\n");

    printf("\tGénération et récupération des données d'une population de %d électeurs donc %d candidats.\n", nv, nc);
    generateRandomData(nv, nc);
    CellProtected* declarations = readProtected("declarations.txt");
    CellKey* voters = readPublicKeys("keys.txt");
    CellKey* candidates = readPublicKeys("candidates.txt");
    printf("\t\tTerminé.\n");

    printf("\tSoumission des votes.\n");
    CellTree* tree = NULL;
    int votes = 0;
    int voteId = 0;
    while (declarations) {
        submitVote(declarations->data);
        ++votes;

        if (votes == 10 || !declarations->next) {
            printf("\t\tCréation et ajout d'un nouveau bloc.\n");
            votes = 0;
            tree = createBlock(tree, declarations->data->votersPublicKey, d);
            char name[16];
            sprintf(name, "%d.txt", voteId++);
            addBlock(d, name);
            printf("\t\t\tTerminé.\n");
        }

        declarations = declarations->next;
    }

    printf("\t\tTerminé.\n");

    printf("\tLecture de l'arbre\n");
    tree = readTree();
    printf("\t\tTerminé.\n");

    printTree(tree);

    printf("\tCalcul du gagnant\n");
    Key* winner = computeWinnerBT(tree, candidates, voters, sizeC, sizeV);
    printf("Clé publique du gagnant : (%lx, %lx)\n", winner->val, winner->n);

    printf("\tFin de la simulation.\n");
    printf("\tLibération de la mémoire.\n");
    deleteListProtected(declarations);
    freeKey(winner);
    deleteCellTree(tree);
    printf("Terminé.\n");
}

void testArborescence() {
    Key x, y, z, a;
    initPairKeys(&x, &y, 3, 7);
    initPairKeys(&z, &a, 3, 7);

    Block block, block2, block3;
    block.previous_hash = NULL;
    block.votes = NULL;
    block.author = &x;

    computeProofOfWork(&block, 1);

    block2.previous_hash = duplicateHash(block.hash);
    block2.votes = NULL;
    block2.author = &y;

    computeProofOfWork(&block2, 1);

    block3.previous_hash = duplicateHash(block2.hash);
    block3.votes = NULL;
    block3.author = &z;

    CellTree* c = createNode(&block);
    CellTree* c2 = createNode(&block2);
    CellTree* c3 = createNode(&block3);

    computeProofOfWork(&block3, 1);

    printTree(c);
    printf("\n");
    addChild(c, c2);
    printTree(c);
    printf("\n");
    addChild(c2, c3);
    printTree(c);
    printf("\n");
};

int main(void) {
    srand(time(NULL));

    test_voteSimulation();

    remove("declarations.txt");
    remove("keys.txt");
    remove("candidates.txt.txt");
    remove(PENDING_VOTES);
    remove(PENDING_BLOCK);

    DIR* blockchainDir = opendir("./Blockchain/");
    struct dirent* dir = NULL;
    while ((dir = readdir(blockchainDir))) {
        if (strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0) {
            char path[128];
            sprintf(path, "./Blockchain/%s", dir->d_name);
            remove(path);
        }
    }
    closedir(blockchainDir);

    return 0;
}