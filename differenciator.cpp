#include "bintree.hpp"
#include "differenciator.hpp"
#include "flashCode.hpp"

int lenFile(FILE *text) {
    fseek(text, 0, SEEK_END);
    int length =  ftell(text);
    fseek(text, 0, SEEK_SET);
    printf ("\nlengthOfFile = %d\n", length);

    return length;    
}

void readTree (struct tree* myTree) {
    FILE* input = fopen ("tree_reader.txt", "r+");

    myTree->length = lenFile (input);
    myTree->allText = (char*) calloc (myTree->length, sizeof(char));
    fread(myTree->allText, sizeof(char), (size_t) (myTree->length), input);
    myTree->allText += 1;

    int typeOfNode = selectType (myTree);
    union value value;

    if (typeOfNode == NUMBER) {
        sscanf (myTree->allText, TYPE_DESIG, &value.number);
    } else if (typeOfNode == OPERATION) {
        sscanf (myTree->allText, "%c", &value.operation);           
    } else if (typeOfNode == VARIABLE) {
        int i = 0;

        while (myTree->allText[i] != ')') {
            value.variable[i] = myTree->allText[i];
            i++;
        }

        value.variable[i] = '\0';
    }

    findScope (myTree);
    treeCtor (myTree, typeOfNode, value);

    fillingTree (myTree, 1);
    fclose (input);
    

}


void fillingTree (struct tree* myTree, int parent) {
    if (myTree->allText[0] == '(') {
        myTree->allText += 1;

        int type = selectType (myTree);
        union value value;

        if (type == NUMBER) {
            sscanf (myTree->allText, TYPE_DESIG, &value.number);
        } else if (type == OPERATION) {
            sscanf (myTree->allText, "%c", &value.operation);
        } else if (type == VARIABLE) {
            int i = 0;

            while (myTree->allText[i] != ')') {
                value.variable[i] = myTree->allText[i];
                i++;
            }

            value.variable[i] = '\0';
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
        return NUMBER;
    } else if (myTree->allText[myTree->currentSym] >= 'a' && myTree->allText[myTree->currentSym] <= 'z') {
        return VARIABLE;           
    }
    return OPERATION;               
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

void diffNode (struct tree* myDiffTree, struct tree* myTree, struct node* n, int parent) {
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
                DL
                DR
                return;
            } else if (n->value.operation == '-') {
                parent = treeAdd (myDiffTree, parent, n->value, OPERATION);
                DL
                DR
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
            myTree->error = ERROR_UNKNOWN_OPERATION;
            printf ("ERROR:Unknown operation!\n");
            return;
            break;  
        }
        case FREE:
        {
            return;
            break;
        }
        default:
        {
            myTree->error = ERROR_UNKNOWN_TYPE;
            printf ("ERROR:Unknown type of node!\n");
            break;
        }
    }
}

void treeCopy (struct tree* myDiffTree, struct tree* myTree, struct node* n, int parent) {
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

            treeCtor (myDiffTree, NUMBER, OneValue);                //return dVAR vars independent
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

                DL       
                DR

                return;
            } else if (n->value.operation == '-') {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                DL
                DR

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
            myTree->error = ERROR_UNKNOWN_OPERATION;
            printf ("ERROR:Unknown operation!\n");
            return;
            break;                     //return one of branching
        }
        default:
        {
            myTree->error = ERROR_UNKNOWN_TYPE;
            printf ("ERROR:Unknown type of node!\n");
            return;
            break;
        }
    }
}


void treeCut (struct tree* myDiffTree, int parent) {
    union value a;
    union value b;

    if (myDiffTree->data[parent].type_of_value == OPERATION) {
        if ((myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == NUMBER) && (myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == NUMBER)) {
            a = treeDel (myDiffTree, parent, LEFTY);
            b = treeDel (myDiffTree, parent, RIGHTY);

            myDiffTree->data[parent].type_of_value = NUMBER;

            switch (myDiffTree->data[parent].value.operation) {
                case ADD:
                {
                    myDiffTree->data[parent].value.number = a.number + b.number;
                    break;
                }

                case SUB:
                {
                    myDiffTree->data[parent].value.number = a.number - b.number;
                    break;
                }
                
                case MUL:
                {
                    myDiffTree->data[parent].value.number = a.number * b.number;
                    break;
                }

                case DIV:
                {
                    if (cmpFloats(b.number, 0)) {
                        myDiffTree->error = ERROR_DIVISION_ON_ZERO;
                        printf ("ERROR: Division on zero\n");
                        return;
                    }

                    myDiffTree->data[parent].value.number = a.number / b.number;
                    break;
                }

                default:
                {
                    myDiffTree->error = ERROR_UNKNOWN_OPERATION;
                    printf ("UNKNOWN OPERATION IN TREE_CUT\n");
                    break;
                }
            }
            
        }

    if (myDiffTree->data[parent].type_of_value == OPERATION) {          //check it if not cutted previosly
        switch (myDiffTree->data[parent].value.operation) {
            case MUL:
            {
                if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == NUMBER && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == VARIABLE) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].lefty].value.number, 0)) {
                        treeDel (myDiffTree, parent, LEFTY);
                        treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = NUMBER;
                        myDiffTree->data[parent].value.number = 0;
                    }

                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].lefty].value.number, 1)) {
                        treeDel (myDiffTree, parent, LEFTY);
                        union value copy = treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                } else if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == VARIABLE && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == NUMBER) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].righty].value.number, 0)) {
                        treeDel (myDiffTree, parent, LEFTY);
                        treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = NUMBER;
                        myDiffTree->data[parent].value.number = 0;
                    } else if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].righty].value.number, 1)) {
                        union value copy = treeDel (myDiffTree, parent, LEFTY);
                        treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                }

                break;
            }

            case ADD:
            {
                if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == NUMBER && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == VARIABLE) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].lefty].value.number, 0)) {
                        treeDel (myDiffTree, parent, LEFTY);
                        union value copy = treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                } else if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == VARIABLE && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == NUMBER) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].righty].value.number, 0)) {
                        union value copy = treeDel (myDiffTree, parent, LEFTY);
                        treeDel (myDiffTree, parent, RIGHTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                }

                break;
            }

            case DIV:
            {
                if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == VARIABLE && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == NUMBER) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].righty].value.number, 1)) {
                        treeDel (myDiffTree, parent, RIGHTY);
                        union value copy = treeDel (myDiffTree, parent, LEFTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                }

                break;
            }

            case SUB:
            {
                if (myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value == VARIABLE && myDiffTree->data[myDiffTree->data[parent].righty].type_of_value == NUMBER) {
                    if (cmpFloats(myDiffTree->data[myDiffTree->data[parent].righty].value.number, 0)) {
                        treeDel (myDiffTree, parent, RIGHTY);
                        union value copy = treeDel (myDiffTree, parent, LEFTY);

                        myDiffTree->data[parent].type_of_value = VARIABLE;
                        myDiffTree->data[parent].value = copy;
                    }
                }

                break;
            }

            default:
            {
                printf ("parent:%d ", parent);
                printf ("op:%c- ", myDiffTree->data[parent].value.operation);
                myDiffTree->error = ERROR_UNKNOWN_OPERATION;
                printf ("UNKNOWN OPERATION IN TREE_CUT\n");
                break;
            }
        };
    }
        if (myDiffTree->data[parent].lefty != 0) {
            treeCut (myDiffTree, myDiffTree->data[parent].lefty);
        }

        if (myDiffTree->data[parent].righty != 0) {
            treeCut (myDiffTree, myDiffTree->data[parent].righty);
        }
    }


    return;
}

union value treeDel (struct tree* myDiffTree, int parent, int child) {
    union value result;
    result.number = -1;
    if (child == LEFTY) {
        myDiffTree->data[myDiffTree->data[parent].lefty].lefty = 0;
        myDiffTree->data[myDiffTree->data[parent].lefty].righty = 0;
        myDiffTree->data[myDiffTree->data[parent].lefty].type_of_value = FREE;

        result = myDiffTree->data[myDiffTree->data[parent].lefty].value;

        myDiffTree->data[myDiffTree->data[parent].lefty].value.number = POISON;
        myDiffTree->free[myDiffTree->data[parent].lefty] = myDiffTree->free_node;
        myDiffTree->free_node = myDiffTree->data[parent].lefty;

        myDiffTree->data[parent].lefty = 0;

        return result;
    } else if (child == RIGHTY) {
        myDiffTree->data[myDiffTree->data[parent].righty].lefty = 0;
        myDiffTree->data[myDiffTree->data[parent].righty].righty = 0;
        myDiffTree->data[myDiffTree->data[parent].righty].type_of_value = FREE;

        result = myDiffTree->data[myDiffTree->data[parent].righty].value;

        myDiffTree->data[myDiffTree->data[parent].righty].value.number = POISON;
        myDiffTree->free[myDiffTree->data[parent].righty] = myDiffTree->free_node;
        myDiffTree->free_node = myDiffTree->data[parent].lefty;

        myDiffTree->data[parent].righty = 0;

        return result;
    } else {
        myDiffTree->error = ERROR_DELETE_UNKNOWN_CHILD;
        printf ("ERROR: OMG... No so much children in node!");

        return result;
    }
}

int cmpFloats (float a, float b) {
    if (abs(a - b) < EPSILON) {
        return 1;
    }

    return 0;
}