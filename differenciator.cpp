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
    } else if (typeOfNode == OPERATION) {
        sscanf (myTree->allText, "%c", &value.operation);           //if parent is number or operation it shouldn't have children
    } else if (typeOfNode == VARIABLE) {
        sscanf (myTree->allText, "%c", &value.variable);
    }                                                               //what if it's another thing -> error

    findScope (myTree);
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
        } else if (type == VARIABLE) {
            sscanf (myTree->allText, "%c", &value.variable);
            printf ("eaten op is %c\n", value.variable);            
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
    } else if (myTree->allText[myTree->currentSym] >= 'a' && myTree->allText[myTree->currentSym] <= 'z') {
        return 2;           // variable 
    }
    return 1;               // operation
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
        fprintf (Equation, "push %.0f\n", elem.value.number);
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

void diffNode (struct tree* myDiffTree, struct tree* myTree, const struct node* n, int parent) {
    graph_dump (myDiffTree);
    switch (n->type_of_value) {
        case NUMBER:
        {
            union value ZeroValue;
            ZeroValue.number = 0;

            treeAdd (myDiffTree, parent, ZeroValue, NUMBER);
            return;
            break;
        }

        case VARIABLE:
        {
            union value OneValue;
            OneValue.number = 1;
            
            treeAdd (myDiffTree, parent, OneValue, NUMBER);                //return dVAR
            return;
            break;
        }
        case OPERATION:
        {
            int left_parent = 0;
            int right_parent = 0;
            int left_left_parent = 0;
            int right_left_parent = 0;

            union value SubValue;
            union value PlusValue;
            union value MulValue;

            if (n->value.operation == '+') {
                parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), parent);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), parent);
                return;
            } else if (n->value.operation == '-') {
                parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), parent);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), parent);
                return;
            } else if (n->value.operation == '*') {
                PlusValue.operation = '+';

                parent =  treeAdd (myDiffTree, parent, PlusValue, OPERATION);
                left_parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), left_parent);

                right_parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->lefty]), right_parent);
                return;
            } else if (n->value.operation == '/') {
                parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                
                SubValue.operation = '-';

                left_parent = treeAdd (myDiffTree, parent, SubValue, OPERATION);

                MulValue.operation = '*';

                left_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), left_left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), left_left_parent);


                right_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), right_left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->lefty]), right_left_parent);

                right_parent = treeAdd (myDiffTree, parent, MulValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);

                return;
                break;
            }
            printf ("ERROR:Unknown operation!\n");
            return;                     //return one of branching
            break;  
        }
        case FREE:
        {
            return;
            break;
        }
        default:
        {
            printf ("ERROR:Unknown type of node!\n");
            break;
        }
    }
}

void treeCopy (struct tree* myDiffTree, struct tree* myTree, const struct node* n, int parent) {
    parent = treeAdd (myDiffTree, parent, n->value, n->type_of_value);

    if (n->lefty != 0) {
        treeCopy (myDiffTree, myTree, &(myTree->data[n->lefty]), parent);
    }

    if (n->righty != 0) {
        treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), parent);
    }

    return;
}

void diffTree (struct tree* myDiffTree, struct tree* myTree, struct node* n) {
    switch (n->type_of_value) {
        case NUMBER:
        {
            union value ZeroValue;
            ZeroValue.number = 0;

            treeCtor (myDiffTree, NUMBER, ZeroValue);
            return;
            break;
        }
        
        case VARIABLE:
        {
            union value OneValue;
            OneValue.number = 1;

            treeCtor (myDiffTree, NUMBER, OneValue);                //return dVAR
            return;
            break;
        }

        case OPERATION:
        {
            int parent = 0;
            int left_parent = 0;
            int right_parent = 0;
            int left_left_parent = 0;
            int right_left_parent = 0;

            union value MulValue;
            union value PlusValue;
            union value SubValue;

            if (n->value.operation == '+') {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), parent);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), parent);

                return;
            } else if (n->value.operation == '-') {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), parent);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), parent);

                return;
            } else if (n->value.operation == '*') {
                PlusValue.operation = '+';

                treeCtor (myDiffTree, OPERATION, PlusValue);
                parent = HEAD;

                left_parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), left_parent);

                right_parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->lefty]), right_parent);

                return;
            } else if (n->value.operation == '/') {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;
                
                SubValue.operation = '-';

                left_parent = treeAdd (myDiffTree, parent, SubValue, OPERATION);

                MulValue.operation = '*';

                left_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->lefty]), left_left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), left_left_parent);


                right_left_parent = treeAdd (myDiffTree, parent, MulValue, OPERATION);
                diffNode (myDiffTree, myTree, &(myTree->data[n->righty]), right_left_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->lefty]), right_left_parent);

                right_parent = treeAdd (myDiffTree, parent, MulValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(myTree->data[n->righty]), right_parent);

                return;
            }
            printf ("ERROR:Unknown operation!\n");
            return;
            break;                     //return one of branching
        }
        default:
        {
            printf ("ERROR:Unknown type of node!\n");
            return;
            break;
        }
    }
}