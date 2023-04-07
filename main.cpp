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

    for (int i = 0; i < 10; i++) {
        treeCut (myTree, HEAD);
    }

    graph_dump (myTree);

    // FILE* Equation = fopen ("CPU/asmeq.asm", "w+");
    // tree_eval (myTree, Equation);

printf ("\n\n\nstart difffff\n\n\n");
    struct tree deadTree;
    struct tree* myDiffTree = &deadTree;
    // treeCtor (myDiffTree, myTree->data[HEAD].type_of_value, myTree->data[HEAD].value);
    diffTree (myDiffTree, myTree, &(myTree->data[1]));
    graph_dump (myDiffTree);


    for (int i = 0; i < 10; i++) {
        //printf ("CUTTING %d\n", i);
        treeCut (myDiffTree, HEAD);
    }
    
    graph_dump (myDiffTree);

printf ("\n\n\nend difffff\n\n\n");
DBG
    // fprintf (Equation, "out\n");
    // fprintf (Equation, "hlt\n");
    // fclose (Equation);
    treeDtor (myTree);
    treeDtor (myDiffTree);

    return 0;
}
// TODO what if operation doesn't exist


//TEST[#1] (+(*(+(x)(y))(*(7)(8)))(-(/(3)(11))(11)))