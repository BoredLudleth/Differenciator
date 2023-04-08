#include "bintree.hpp"
#include "differenciator.hpp"

int main () 
{
    struct tree Tree;
    struct tree* myTree = &Tree;

    readTree (myTree);
    graph_dump (myTree);
    treeCut (myTree, HEAD);
    graph_dump (myTree);

    // [#1]CUTTING ORIGIN TREE
    // for (int i = 0; i < 10; i++) {
    //     treeCut (myTree, HEAD);
    // }

    // [#2]Solving without x with CPU
    // FILE* Equation = fopen ("CPU/asmeq.asm", "w+");
    // tree_eval (myTree, Equation);

    //Differenciation
    struct tree deadTree;
    struct tree* myDiffTree = &deadTree;

    diffTree (myDiffTree, myTree, &(myTree->data[1]));
    graph_dump (myDiffTree);

    for (int i = 0; i < 10; i++) {
        treeCut (myDiffTree, HEAD);
    }

    graph_dump (myDiffTree);

    treeDtor (myTree);
    treeDtor (myDiffTree);

    return 0;
}
// TODO what if operation doesn't exist


//TEST[#1] (+(*(+(x)(y))(*(7)(8)))(-(/(3)(11))(11)))