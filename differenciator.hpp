#ifndef DIFFERENCIATOR_HPP
#define DIFFERENCIATOR_HPP

#include "bintree.hpp"
#include <math.h>

#define HEAD 1

#define LEFTY 0
#define RIGHTY 1

#define EPSILON 1e-5

enum operations {
    ADD = '+',
    SUB = '-',
    DIV = '/',
    MUL = '*',
    //ERROR = 0,
    //POW = '^',
    //SIN = 1,
    //COS
    //TG
    //SQRT
};

void readTree (struct tree* myTree);

void fillingTree (struct tree* myTree, int parent);

int lenFile(FILE *text);

int selectType (struct tree* myTree);

void findScope (struct tree* myTree);

void tree_eval (struct tree* myTree, FILE* Equation, int myTreeHead = HEAD);

void print_node (struct node elem, FILE* Equation);

void diffNode (struct tree* myDiffTree, struct tree* myTree, struct node* n, int parent = HEAD);

void diffTree (struct tree* myDiffTree, struct tree* myTree, struct node* n);

void treeCopy (struct tree* myDiffTree, struct tree* myTree, struct node* firstNode, int parent);

struct tree* treeCut (struct tree* myDiffTree, int parent = HEAD);

void treeReduction (struct tree* myDiffTree);

int treeCmp (struct tree* srcTree, struct tree* dstTree);

union value treeDel (struct tree* myDiffTree, int parent, int child);

int cmpFloats (float a, float b);

#endif
