#ifndef DIFFERENCIATOR_HPP
#define DIFFERENCIATOR_HPP

#include "bintree.hpp"

#define HEAD 1

void readTree (struct tree* myTree);

void fillingTree (struct tree* myTree, int parent);

int lenFile(FILE *text);

int selectType (struct tree* myTree);

void findScope (struct tree* myTree);

void tree_eval (struct tree* myTree, FILE* Equation, int myTreeHead = 1);

void print_node (struct node elem, FILE* Equation);

#endif
