#ifndef BINTREE_HPP
#define BINTREE_HPP

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#define DBG printf("FILE:%s FUNC:%s LINE:%d\n", __FILE__, __PRETTY_FUNCTION__, __LINE__);

#define POISON 0xBAD

typedef float type;
#define TYPE_DESIG "%f"
#define TYPE_DESIG_FOR_PRINT "%5f"
#define MAX_FILE_NAME 40

#define MAX_NODES 10

union value{
    float number;
    char operation;
    char variable;
};

enum TypeOfValue {
    FREE = -1,
    NUMBER    = 0,
    OPERATION = 1,
    VARIABLE  = 2
};

struct node
{
    int type_of_value = FREE;
    union value value;


    int lefty = 0;
    int righty = 0;
};

struct tree 
{   
    int size = MAX_NODES;
    size_t error  = 0;


    struct node* head = nullptr;
    struct node* data = nullptr;    
    
    int* free = nullptr;
    int free_node = 0;

    FILE* output = nullptr;
    char* allText = nullptr;
    int length = 0;
    int currentSym = 0;

    FILE* log_file = nullptr;                 //upgrate log-file

    //add depth for cutting max cycle?
};


void treeCtor (struct tree* myTree, int type, union value value);

void treeDtor (struct tree* myTree);

void treeResize (struct tree* myTree);

int treeAdd (struct tree* myTree, int parent, union value value, int type);

char* inttoa(int n, char* s);

void reverse(char* s);

void graph_dump (struct tree* myTree);

void compileDot (char* name_of_file, char* pic_name);

#endif
