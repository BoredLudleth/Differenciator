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

    FILE* Equation = fopen ("asmeq.asm", "w+");
    tree_eval (myTree, Equation);
DBG
    treeDtor (myTree);

    return 0;
}
// TODO what if operation doesn't exist
