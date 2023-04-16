#include "bintree.hpp"
#include "differenciator.hpp"

#include "recurrsiveDown.hpp"

int main () 
{
    struct tree Tree;
    struct tree* myTree = &Tree;

    readTree (myTree);
    graph_dump (myTree);
    treeReduction (myTree);
    graph_dump (myTree);

    // [#2]Solving without x with CPU
    // FILE* Equation = fopen ("CPU/asmeq.asm", "w+");
    // tree_eval (myTree, Equation);

    //Differenciation
    //prev version (*(+(x)(x))(+(xD)(0)))
    struct tree deadTree;
    struct tree* myDiffTree = &deadTree;

    diffTree (myDiffTree, myTree, &(myTree->data[1]));
    graph_dump (myDiffTree);

    treeReduction (myDiffTree);

    graph_dump (myDiffTree);

    treeDtor (myTree);
    treeDtor (myDiffTree);

    printf ("out:%d\n", getG ("0/(0+1)$"));

    return 0;
}
// TODO what if operation doesn't exist
// TODO add sin, cos and other
// TODO integrate checks? recursion is a problem would be slower... and with exit... +
// TODO return dx, not 1
// TODO ctor not make new log file to down realloced
// TODO TEX-file

//TEST[#1] (+(*(+(x)(y))(*(7)(8)))(-(/(3)(11))(11)))