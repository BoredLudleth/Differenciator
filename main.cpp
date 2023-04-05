#include "bintree.hpp"
#include "differenciator.hpp"

int main () 
{
    struct tree Tree;
    struct tree* myTree = &Tree;

DBG
    readTree (myTree);
DBG
    graph_dump (myTree);

    FILE* Equation = fopen ("CPU/asmeq.asm", "w+");
    tree_eval (myTree, Equation);
DBG
    fprintf (Equation, "out\n");
    fprintf (Equation, "hlt\n");
    fclose (Equation);
    treeDtor (myTree);

    return 0;
}
// TODO what if operation doesn't exist
