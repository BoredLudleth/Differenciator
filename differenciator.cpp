#include "bintree.hpp"
#include "differenciator.hpp"

int lenFile(FILE *text) {
    fseek(text, 0, SEEK_END);
    int length =  ftell(text);
    fseek(text, 0, SEEK_SET);
    printf ("\nlengthOfFile = %d\n", length);

    return length;    
}

void readTree (struct tree* myTree) {
    FILE* input = fopen ("tree_reader.txt", "r+");
DBG
    myTree->length = lenFile (input);
DBG
    myTree->allText = (char*) calloc (myTree->length, sizeof(char));
DBG
    fread(myTree->allText, sizeof(char), (size_t) (myTree->length), input);
DBG
    myTree->allText += 1;
    int typeOfNode = selectType (myTree);
    union value value;
DBG
    if (typeOfNode == NUMBER) {
        sscanf (myTree->allText, TYPE_DESIG, &value.number);
        findScope (myTree);
    } else if (typeOfNode == OPERATION) {
        sscanf (myTree->allText, "%c", &value.operation);
        findScope (myTree);
    }
DBG
    treeCtor (myTree, typeOfNode, value);
DBG
    fillingTree (myTree, 1);
    // '(' создаю узел
    // сразу же записываю его внутренность - следующий символ
    // если внутренность число - усё, не добавляю дальше
    // создаю узел первого сына если есть (
    fclose (input);
    

}


void fillingTree (struct tree* myTree, int parent) {
    DBG
    if (myTree->allText[0] == '(') {
        myTree->allText += 1;

        int type = selectType (myTree);
        union value value;

        if (type == NUMBER) {
            sscanf (myTree->allText, TYPE_DESIG, &value.number);
            printf ("eaten number is %f\n", value.number);
        } else if (type == OPERATION) {
            sscanf (myTree->allText, "%c", &value.operation);
            printf ("eaten op is %c\n", value.operation);
        }

        findScope (myTree);

        int newparent = treeAdd (myTree, parent, value, type);
        fillingTree (myTree, newparent); 
        fillingTree (myTree, newparent);
        
        if (myTree->allText[0] == ')') {
            myTree->allText += 1;
            fillingTree (myTree, parent);
        }

    }

}

int selectType (struct tree* myTree) {
    if (myTree->allText[myTree->currentSym] <= '9' && myTree->allText[myTree->currentSym] >= '0') {
        return 0;
    }
    return 1;
}

void findScope (struct tree* myTree) {
    while ((myTree->allText[0] != '(') && (myTree->allText[0] != ')')) {
        myTree->allText += 1;
    }
}

void tree_eval (struct tree* myTree, FILE* Equation, int myTreeHead) {
    if (myTree->data[myTreeHead].lefty != 0) {
        tree_eval (myTree, Equation, myTree->data[myTreeHead].lefty);
    }


    if (myTree->data[myTreeHead].righty != 0) {
        tree_eval (myTree, Equation, myTree->data[myTreeHead].righty);
    }

    print_node (myTree->data[myTreeHead], Equation);
}

void print_node (struct node elem, FILE* Equation) {
    if (elem.type_of_value == NUMBER) {
        fprintf (Equation, "push %f\n", elem.value.number);
    } else if (elem.type_of_value == OPERATION) {
        switch (elem.value.operation) {
            case '*':
                fprintf (Equation, "mul\n");
                break;
            case '+':
                fprintf (Equation, "add\n");
                break;
            case '-':
                fprintf (Equation, "sub\n");
                break;
            case '/':
                fprintf (Equation, "div\n");
                break;
            default:
                printf ("Operation doesn't exist\n");
                return;
        }
    } else {
        printf ("Value isn't operation and not number\n");
        return;
    }
}