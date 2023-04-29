#include "bintree.hpp"
#include "differenciator.hpp"

#include "recurrsiveReader.hpp"

const char* S = NULL;

int p = 0;

void treeRecCtor (struct tree* myTree, FILE* old_log) {
    static int count_tree = 0;

    if (old_log == nullptr) {
        printf ("NEW LOG\n");
        char log_name[15] = "log";
        char number_of_log[5] = "";
        strcat (log_name, inttoa(count_tree, number_of_log));
        strcat (log_name, ".html");

        myTree->log_file = fopen (log_name, "w+");

        count_tree++;
    } else {
        myTree->log_file = old_log;
    }

    fprintf (myTree->log_file, "<pre>\n");
    
    fprintf (myTree->log_file, "<h2>Tree Ctor started</h2>\n");

    myTree->size = 10;

    myTree->data = (node*) calloc (myTree->size, sizeof (node));
    myTree->free = (int*) calloc (myTree->size, sizeof (int));

    myTree->free[0] = -1;
    myTree->free[1] = -1;
    myTree->free_node = 1;

    for (int i = 1; i < myTree->size - 1; i++) {
        myTree->free[i] = i + 1;
        myTree->data[i].type_of_value = FREE;
    }

    myTree->length = 0;

    myTree->free[myTree->size - 1] = 0;

    myTree->head = myTree->data;

    fprintf (myTree->log_file, "Tree Ctor complete\n");
}

int treeRecAdd (struct tree* myTree, union value value, int type) {
    if (myTree->free_node == 0) {
        treeResizeUp(myTree);
    }

    int index = myTree->free_node;

    if (type == OPERATION) {
        myTree->data[myTree->free_node].value = value; 
    } else  if (type == NUMBER){
        myTree->data[myTree->free_node].value.number = value.number;
    } else if (type == VARIABLE) {
        myTree->data[myTree->free_node].value = value;
    } else {
        printf ("ERROR, UNKNOWN TYPE\n");
        exit (0);
    }

    myTree->data[index].type_of_value  = type;
    myTree->length++;

    myTree->free_node =  myTree->free[index];
    myTree->free[index] = -1;

    return index;
}

void reccursiveReader (struct tree* myTree) {
    treeRecCtor (myTree);
    FILE* toRead = fopen ("tree_reader.txt", "r");

    myTree->length = lenFile (toRead);
    myTree->allText = (char*) calloc (myTree->length, sizeof(char));
    fread(myTree->allText, sizeof(char), (size_t) (lenFile(toRead)), toRead);
    fclose (toRead);

    printf ("%s", myTree->allText);

    int head = readG (myTree, myTree->allText);

    treeReBuild (myTree, head);
}

void treeReBuild (struct tree* myTree, int head) {
    struct tree reallocTree;
    struct tree* copyTree = &reallocTree;

    treeCtor (copyTree, myTree->data[head].type_of_value, myTree->data[head].value, myTree->log_file);

    if (myTree->data[head].lefty != 0) {
        treeCopy (copyTree, myTree, &(myTree->data[myTree->data[head].lefty]), HEAD);
    }
    if (myTree->data[head].righty != 0) {
        treeCopy (copyTree, myTree, &(myTree->data[myTree->data[head].righty]), HEAD);
    }
    
    treeDtor (myTree);

    *myTree = reallocTree;

    fprintf (myTree->log_file, "Tree resized down\n");
}

int readG (struct tree* myTree, const char* str) {//get end of str
    printf ("I'm in readG\n");

    S = str;
    p = 0;
    int head = readE(myTree);
    myTree->head = &(myTree->data[head]);
    if (S[p] == '$') {
        printf ("I'm out readG\n");
        return head;
    }
}

int readN (struct tree* myTree) {
    printf ("I'm in readN\n");       //get number

    float val = 0;
    int savP = 0;
    do {
        val = val * 10 + S[p] - '0';
        p++;
    } while ('0' <= S[p] && S[p] <= '9');

    int countGradeTen = 1;

    if (S[p] == '.') {
        p++;
        do {
            val += (S[p] - '0') / pow (10, countGradeTen);
            countGradeTen++;
            p++;
        } while ('0' <= S[p] && S[p] <= '9');
    }

    savP = p;

    union value result = {.number = val};

    printf ("I'm out getN\n");

    return treeRecAdd (myTree, result, NUMBER);
}

int readX (struct tree* myTree) {
    printf ("I'm in readX\n");       //get number

    char val[3] = {};
    int savP = 0;
    int numOfChar = 0;
    if (('A' <= S[p] && S[p] <= 'Z') || ('a' <= S[p] && S[p] <= 'z')) {
        do {
            val[numOfChar] = S[p];
            p++;
        } while (('A' <= S[p] && S[p] <= 'Z') || ('a' <= S[p] && S[p] <= 'z'));
    } else {
        return readN (myTree);
    }

    savP = p;

    union value result = {.variable = ""};

    strncpy (result.variable, val, 4);

    printf ("I'm out getN\n");

    return treeRecAdd (myTree, result, VARIABLE);
}

int readP (struct tree* myTree) {
    printf ("%s", S);
    printf ("I'm in readP\n");
    if (S[p] == '(') {//scopes
        p++;
        int index = readE(myTree);
        if (S[p] == ')') {
            p++;
        } else {
            printf ("error readP\n");
            exit (0);
        }

        printf ("I'm out readP\n");
        
        return index;
    } else {
        printf ("I'm out readP go to readX\n");
        return readX (myTree);
    }
}

int readT (struct tree* myTree) {
    printf ("I'm in getT\n");
    int index1 = readS (myTree);
    
    while (S[p] == '*' || S[p] == '/' || S[p] == '^') {// mul and div
        union value op = {.operation = ""};
        op.operation[0] = S[p];
        p++;
        int index2 = readS (myTree);

        int index = treeRecAdd (myTree, op, OPERATION);
        myTree->data[index].lefty  = index1;
        myTree->data[index].righty = index2;

        index1 = index;
    }
    printf ("I'm out getT\n");
    return index1;
}

int readE (struct tree* myTree) {
    printf ("I'm in readE\n");

    int index1 = readT (myTree);
    
    while (S[p] == '+' || S[p] == '-') {//sum, sub
        union value op = {.operation = ""};
        op.operation[0] = S[p];
        p++;
        int index2 = readT (myTree);

        int index = treeRecAdd (myTree, op, OPERATION);
        myTree->data[index].lefty  = index1;
        myTree->data[index].righty = index2;
        index1 = index;
    }

    printf ("I'm out readE\n");
    
    return index1;
}

int readS (struct tree* myTree) {
    printf ("I'm in readS\n");

    union value shOp = {.number = 0};

    if (!(strncmp (S + p, "sin", 3) * strncmp (S + p, "cos", 3) * strncmp (S + p, "tg", 2) * strncmp (S + p, "ln", 2))) {
        int index1 = 0;
        while (!(strncmp (S + p, "sin", 3) * strncmp (S + p, "cos", 3) * strncmp (S + p, "tg", 2) * strncmp (S + p, "ln", 2))) {
            shOp.number = 0;
            if (!strncmp (S + p, "sin", 3)) {
                strcpy(shOp.operation, "sin");
                p += 3;
            } else if (!strncmp (S + p, "cos", 3)) {
                strcpy(shOp.operation, "cos");
                p += 3;
            } else if (!strncmp (S + p, "tg", 2)) {
                strcpy(shOp.operation, "tg");
                p += 2;
            } else if (!strncmp (S + p, "ln", 2)) {
                strcpy(shOp.operation, "ln");
                p += 2;
            }
            int index = treeRecAdd (myTree, shOp, OPERATION);
            index1 = readP (myTree);
            myTree->data[index].lefty  = index1;
            index1 = index;
        }
        printf ("I'm out readS\n");
        
        return index1;
    } else {
        return readP (myTree);
    }
}
