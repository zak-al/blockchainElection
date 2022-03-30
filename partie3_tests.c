//
// Created by Zhichun Hua on 29/03/2022.
//

#include "stdlib.h"
#include "stdio.h"
#include "rsa.h"
#include "voting.h"

int main() {
//Partie 3
    printf("tests de la partie 3\n");

//tests pour Question 5.1 et Question 5.2
    Key *test = malloc(sizeof(Key));
    init_key(test, 5, 7);

    CellKey * list_cell = create_cell_key(test);
    list_cell = prependKey(test, list_cell);
    list_cell = prependKey(test, list_cell);

//tests pour Question 5.3
    print_list_keys(list_cell);

//tests pour Question 5.4
    CellKey *ls_key = read_public_keys("keys.txt");
    CellKey *ls_candidate = read_public_keys("candidates.txt");
//afficher les resultats
    print_list_keys(ls_key);
    print_list_keys(ls_candidate);

//tests pour Question 5.5
    Key *test2 = malloc(sizeof(Key));
    init_key(test2, 6, 9);
    list_cell = prependKey(test2, list_cell);
    list_cell = prependKey(test, list_cell);
    printf("avant de supprimer : \n");
    print_list_keys(list_cell);
    printf("apres avoir supprime : \n");
    delete_list_keys(list_cell);
    //print_list_keys(list_cell);

/*tests pour Question 5.6 et Question 5.7
    Signature *sgn1 = init_signature(NULL, 0);
    Protected *pr1 = init_protected(test, "test", sgn1);
    Protected *pr2 = init_protected(test2, "test2", sgn1);
    CellProtected *list_pro = NULL;

    prependProtected(pr1, list_pro);
    prependProtected(pr1, list_pro);
    prependProtected(pr1, list_pro);

//tests pour Question 5.9
    print_list_protected(list_pro);

//tests pour Question 5.8
    CellProtected *ls_pro = read_protected("declarations.txt");
//afficher les resultats
    print_list_protected(ls_pro);

//tests pour Question 5.5
    prependProtected(pr2,list_pro);
    prependProtected(pr1,list_pro);
    printf("avant de supprimer : \n");
    print_list_protected(list_pro);
    printf("apres avoir supprime : \n");
    print_list_protected(list_pro);
*/
}