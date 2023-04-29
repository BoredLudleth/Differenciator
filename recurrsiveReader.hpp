#ifndef RECCURSIVE_READER_HPP
#define RECCURSIVE_READER_HPP

#include "bintree.hpp"
#include "differenciator.hpp"

void reccursiveReader (struct tree* myTree);

void treeRecCtor (struct tree* myTree, FILE* old_log = nullptr);

int treeRecAdd (struct tree* myTree, union value value, int type);

void treeReBuild (struct tree* myTree, int head);

int readG (struct tree* myTree, const char* str);

int readE (struct tree* myTree);

int readT (struct tree* myTree);

int readS (struct tree* myTree);

int readP (struct tree* myTree);

int readX (struct tree* myTree);

int readN (struct tree* myTree);



#endif