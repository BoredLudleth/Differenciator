#include "bintree.hpp"
#include "differenciator.hpp"

#include "recurrsiveReader.hpp"

int main () 
{
    struct tree Tree;
    struct tree* myTree = &Tree;

    FILE* f = fopen ("answer.tex", "w+");
    reccursiveReader (myTree);
    fprintf (f, "without cut\n\\\\\n");
    writeTex (myTree, HEAD, f);
    fprintf (f, "\n\\\\\n");
    graph_dump (myTree);

    fprintf (f, "cut\n\\\\\n");
    treeReduction (myTree);
    graph_dump (myTree);
    writeTex (myTree, HEAD, f);
    fprintf (f, "\n\\\\\n");

    struct tree deadTree;
    struct tree* myDiffTree = &deadTree;

    fprintf (f, "diff without cut\n\\\\\n");
    diffTree (myDiffTree, myTree, &(myTree->data[1]));
    graph_dump (myDiffTree);
    writeTex (myDiffTree, HEAD, f);
    fprintf (f, "\n\\\\\n");

    fprintf (f, "diff with cut\n\\\\\n");
    treeReduction (myDiffTree);
    graph_dump (myDiffTree);
    writeTex (myDiffTree, HEAD, f);
    fprintf (f, "\n\\\\\n");

    treeDtor (myTree);
    treeDtor (myDiffTree);
    fclose (f);

    return 0;
}
// TODO flash code

//TEST[#1] (+(*(+(x)(y))(*(7)(8)))(-(/(3)(11))(11)))