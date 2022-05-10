#ifndef PROJETSTR_SIMULATION_H
#define PROJETSTR_SIMULATION_H

#include "Protected.h"
#include "Arborescence.h"

void submitVote(Protected* p);
CellTree* createBlock(CellTree* cellTree, Key* author, int d);
void addBlock(int d, char* name);
CellTree* readTree(void);
Key* computeWinnerBT(CellTree* tree, CellKey* candidates, CellKey* voters, int sizeC, int sizeV);

#endif //PROJETSTR_SIMULATION_H
