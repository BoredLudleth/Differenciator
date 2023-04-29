#include "bintree.hpp"
#include "differenciator.hpp"
#include "flashCode.hpp"

int lenFile(FILE *text) {
    fseek(text, 0, SEEK_END);
    int length = (int) ftell(text);
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
        int i = 0;

        while (myTree->allText[i] != '(') {
            value.operation[i] = myTree->allText[i];
            i++;
        }

        value.operation[i] = '\0';

    } else if (typeOfNode == VARIABLE) {
        int i = 0;
        strcpy (value.variable, "\0\0\0\0");
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
            int i = 0;

            while (myTree->allText[i] != '(') {
                value.operation[i] = myTree->allText[i];
                i++;
            }

            value.operation[i] = '\0';
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
        myTree->length++;

        if (type == OPERATION) {
            fillingTree (myTree, newparent); 
            fillingTree (myTree, newparent);
        }

        if (myTree->allText[0] == ')') {
            myTree->allText += 1;
            fillingTree (myTree, parent);
        }

    }

}

int selectType (struct tree* myTree) {
    if (myTree->allText[myTree->currentSym] <= '9' && myTree->allText[myTree->currentSym] >= '0') {
        return NUMBER;
    } else {
        if (myTree->allText[myTree->currentSym] == '+' || myTree->allText[myTree->currentSym] ==  '-' || myTree->allText[myTree->currentSym] == '/' ||
            myTree->allText[myTree->currentSym] == '*' || myTree->allText[myTree->currentSym] == '^') {
                return OPERATION;
        }

        if (!strncmp (&(myTree->allText[myTree->currentSym]), "sin", 3)) {
            return OPERATION;
        }

        if (!strncmp (&(myTree->allText[myTree->currentSym]), "cos", 3)) {
            return OPERATION;
        }

        if (!strncmp (&(myTree->allText[myTree->currentSym]), "tg", 2)) {
            return OPERATION;
        }

        if (!strncmp (&(myTree->allText[myTree->currentSym]), "sqr", 3)) {
            return OPERATION;
        } 

        if (!strncmp (&(myTree->allText[myTree->currentSym]), "ln", 2)) {
            return OPERATION;
        }    
    }
    return VARIABLE;               
}

void findScope (struct tree* myTree) {
    while ((myTree->allText[0] != '(') && (myTree->allText[0] != ')')) {
        myTree->allText += 1;
    }
}

void tree_eval (struct tree* myTree, FILE* Equation, int myTreeHead) {
    if (TREE_DATA[myTreeHead].lefty != 0) {
        tree_eval (myTree, Equation, TREE_DATA[myTreeHead].lefty);
    }


    if (TREE_DATA[myTreeHead].righty != 0) {
        tree_eval (myTree, Equation, TREE_DATA[myTreeHead].righty);
    }

    print_node (TREE_DATA[myTreeHead], Equation);
}

void print_node (struct node elem, FILE* Equation) {
    if (elem.type_of_value == NUMBER) {
        fprintf (Equation, "push %.0f\n", elem.value.number);
    } else if (elem.type_of_value == OPERATION) {
        switch (elem.value.operation[0]) {
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
            //other cases!
            default:
                printf ("Operation doesn't exist\n");
                return;
        }
    } else {
        printf ("Value isn't operation and not number\n");
        return;
    }
}

void diffNode (struct tree* myDiffTree, struct tree* myTree, struct node* n, char* dVar, int parent) {
    char diffVar[4] = {};
    diffVar[0] = 'd';
    strcat (diffVar, dVar);
    strcat (diffVar, "\0");

    union value dVarUn = {.variable = {}};
    strcpy (dVarUn.variable, diffVar);
    strcat (dVarUn.variable, "\0");

    union value ZeroValue = {.number = 0.f};

    switch (n->type_of_value) {
        case NUMBER:
        {
            myDiffTree->length += 1;

            treeAdd (myDiffTree, parent, ZeroValue, NUMBER);
            return;
            break;
        }

        case VARIABLE:
        {
            if (!strcmp (n->value.variable, dVar)) {
                treeAdd (myDiffTree, parent, dVarUn, VARIABLE);
                printf ("D TO X\n");
            } else {
                treeAdd (myDiffTree, parent, ZeroValue, NUMBER);
            }

            return;
            break;
        }
        case OPERATION:                                                                         //add others!
        {
            int left_parent = 0;
            int right_parent = 0;
            int left_left_parent = 0;
            int right_left_parent = 0;

            union value SubValue =  {.operation = "-"};
            union value PlusValue = {.operation = "+"};
            union value MulValue = {.operation = "*"};
            union value DivValue = {.operation = "/"};
            union value SinValue = {.operation = "sin"};
            union value CosValue = {.operation = "cos"};
            union value PowValue = {.operation = "^"};
            union value LnValue = {.operation = "ln"};

            union value OneValue = {.number = 1};

            if (!strcmp (n->value.operation, "+")) {
                myDiffTree->length += 1;

                parent = ADD_OPERATION
                DL
                DR
                return;
            } else if (!strcmp (n->value.operation, "-")) {
                myDiffTree->length += 1;

                parent = ADD_OPERATION
                DL
                DR
                return;
            } else if (!strcmp (n->value.operation, "*")) {

                parent =  TREE_ADD

                myDiffTree->length += 1;

                left_parent = ADD_OPERATION

                myDiffTree->length += 1;

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_parent);

                right_parent = ADD_OPERATION
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), right_parent);
                return;
            } else if (!strcmp (n->value.operation, "/")) {
                parent = ADD_OPERATION

                myDiffTree->length += 1;

                left_parent = TREE_SUB

                myDiffTree->length += 1;

                left_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);

                myDiffTree->length += 1;

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_left_parent);


                right_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);

                myDiffTree->length += 1;

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), right_left_parent);

                right_parent = TREE_MUL

                myDiffTree->length += 1;

                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_parent);

                return;
                break;
            } else if (!strcmp (n->value.operation, "ln")) {
                parent = TREE_MUL

                left_parent = TREE_DIV
                treeAdd (myDiffTree, left_parent, OneValue, NUMBER);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[left_parent]), left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "sin")) {
                parent = TREE_MUL

                left_parent = treeAdd (myDiffTree, parent, CosValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "cos")) {
                parent = TREE_MUL

                left_parent = TREE_SUB
                treeAdd (myDiffTree, left_parent, ZeroValue, NUMBER);
                left_left_parent = treeAdd (myDiffTree, left_parent, SinValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "tg")) {
                parent = TREE_MUL

                left_parent = TREE_DIV
                treeAdd (myDiffTree, left_parent, OneValue, NUMBER);
                left_left_parent = treeAdd (myDiffTree, left_parent, PowValue, OPERATION);
                int left_left_left_parent = treeAdd (myDiffTree, left_left_parent, CosValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_left_left_parent);
                treeAdd (myDiffTree, left_left_parent, {.number = 2}, NUMBER);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "^")) {
                parent = TREE_ADD
                left_parent = TREE_MUL
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_parent);
                int left_right_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION); 
                int left_right_left_parent = treeAdd (myDiffTree, left_right_parent, PowValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_right_left_parent);
                int left_right_left_right = treeAdd (myDiffTree, left_right_left_parent, SubValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_right_left_right);
                treeAdd (myDiffTree, left_right_left_right, OneValue, NUMBER); 
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_right_parent);

                right_parent = TREE_MUL
                treeCopy (myDiffTree, myTree, n, right_parent);   
                int right_right_parent = treeAdd (myDiffTree, right_parent, MulValue, OPERATION);
                int right_right_left_parent = treeAdd (myDiffTree, right_right_parent, LnValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_right_left_parent);
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_right_parent);

                return;
                break;
            }

            myTree->error = ERROR_UNKNOWN_OPERATION;
            printf ("ERROR:Unknown operation! in function DifNode\n");
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
            printf ("ERROR:Unknown type of node! default in DifNode\n");
            break;
        }
    }
}

void treeCopy (struct tree* myDiffTree, struct tree* myTree, struct node* firstNode, int parent) {
    parent = treeAdd (myDiffTree, parent, firstNode->value, firstNode->type_of_value);

    myDiffTree->length += 1;

    if (firstNode->lefty != 0) {
        treeCopy (myDiffTree, myTree, &(TREE_DATA[firstNode->lefty]), parent);
    }

    if (firstNode->righty != 0) {
        treeCopy (myDiffTree, myTree, &(TREE_DATA[firstNode->righty]), parent);
    }

    return;
}

void diffTree (struct tree* myDiffTree, struct tree* myTree, struct node* n) {
    char dVar[4] = {};
    printf ("On which variable do you want to differentiate?\n");
    scanf ("%s", dVar);
    strcat (dVar, "\0");

    char diffVar[4] = {};
    strcat (diffVar, "d");
    strncat (diffVar, dVar, 4);

    union value dVarUn = {.variable = {}};
    strcpy (dVarUn.variable, diffVar);

    switch (n->type_of_value) {
        case NUMBER:
        {
            union value ZeroValue = {.number = 0.f};

            treeCtor (myDiffTree, NUMBER, ZeroValue, nullptr);

            return;
            break;
        }
        
        case VARIABLE:
        {
            if (!strcmp (n->value.variable, dVar)) {
                treeCtor (myDiffTree, VARIABLE, dVarUn);
            } else {
                union value ZeroValue = {.number = 0.f};
                treeCtor (myDiffTree, NUMBER, ZeroValue);                //return dVAR vars independent
            }

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

            union value SubValue =  {.operation = "-"};
            union value PlusValue = {.operation = "+"};
            union value MulValue = {.operation = "*"};
            union value DivValue = {.operation = "/"};
            union value SinValue = {.operation = "sin"};
            union value CosValue = {.operation = "cos"};
            union value PowValue = {.operation = "^"};
            union value LnValue = {.operation = "ln"};

            union value OneValue = {.number = 1};
            union value ZeroValue = {.number = 0};

            if (!strcmp (n->value.operation, "+")) {                                                                     //add others!!!!!!!!!!!!!!!!!!!!!!!
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                DL       
                DR

                return;
            } else if (!strcmp (n->value.operation, "-")) {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                DL
                DR

                return;
            } else if (!strcmp (n->value.operation, "*")) {
                treeCtor (myDiffTree, OPERATION, PlusValue);
                parent = HEAD;

                left_parent = ADD_OPERATION
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_parent);

                right_parent = ADD_OPERATION
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), right_parent);

                return;
            } else if (!strcmp (n->value.operation,  "/")) {
                treeCtor (myDiffTree, OPERATION, n->value);
                parent = HEAD;

                left_parent = TREE_SUB

                left_left_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION);
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_left_parent);


                right_left_parent = TREE_MUL
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_left_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), right_left_parent);

                right_parent = TREE_MUL
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_parent);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_parent);

                return;
            } else if (!strcmp (n->value.operation, "ln")) {
                treeCtor (myDiffTree, OPERATION, MulValue);
                parent = HEAD;

                left_parent = TREE_DIV
                treeAdd (myDiffTree, left_parent, OneValue, NUMBER);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[left_parent]), left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "sin")) {
                treeCtor (myDiffTree, OPERATION, MulValue);
                parent = HEAD;

                left_parent = treeAdd (myDiffTree, parent, CosValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "cos")) {
                treeCtor (myDiffTree, OPERATION, MulValue);
                parent = HEAD;


                left_parent = TREE_SUB
                treeAdd (myDiffTree, left_parent, ZeroValue, NUMBER);
                left_left_parent = treeAdd (myDiffTree, left_parent, SinValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_left_parent);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "tg")) {
                treeCtor (myDiffTree, OPERATION, MulValue);
                parent = HEAD;


                left_parent = TREE_DIV
                treeAdd (myDiffTree, left_parent, OneValue, NUMBER);
                left_left_parent = treeAdd (myDiffTree, left_parent, PowValue, OPERATION);
                int left_left_left_parent = treeAdd (myDiffTree, left_left_parent, CosValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_left_left_parent);
                treeAdd (myDiffTree, left_left_parent, {.number = 2}, NUMBER);

                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);
                return;
                break;
            } else if (!strcmp (n->value.operation, "^")) {
                treeCtor (myDiffTree, OPERATION, PlusValue);
                parent = HEAD;

                left_parent = TREE_MUL
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_parent);
                int left_right_parent = treeAdd (myDiffTree, left_parent, MulValue, OPERATION); 
                int left_right_left_parent = treeAdd (myDiffTree, left_right_parent, PowValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->lefty]), left_right_left_parent);
                int left_right_left_right = treeAdd (myDiffTree, left_right_left_parent, SubValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), left_right_left_right);
                treeAdd (myDiffTree, left_right_left_right, OneValue, NUMBER); 
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, left_right_parent);

                right_parent = TREE_MUL
                treeCopy (myDiffTree, myTree, n, right_parent);   
                int right_right_parent = treeAdd (myDiffTree, right_parent, MulValue, OPERATION);
                int right_right_left_parent = treeAdd (myDiffTree, right_right_parent, LnValue, OPERATION);
                treeCopy (myDiffTree, myTree, &(TREE_DATA[n->righty]), right_right_left_parent);
                diffNode (myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, right_right_parent);

                return;
                break;
            }


            myTree->error = ERROR_UNKNOWN_OPERATION;
            printf ("ERROR:Unknown operation!\n");
            return;
            break;                     //return one of branching                                                            ???????????????????
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

struct tree* treeCut (struct tree* myDiffTree, int parent) {
    union value a = {.number = 0.f};
    union value b = {.number = 0.f};

    if (DIFF_PARENT.type_of_value == OPERATION) {
        if ((myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER) && (myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER)) {
            a = TREE_DEL_LEFT
            b = TREE_DEL_RIGHT

            myDiffTree->length -= 2;

            DIFF_PARENT.type_of_value = NUMBER;

            switch (DIFF_PARENT.value.operation[0]) {
                case ADD:
                {
                    DIFF_PARENT.value.number = a.number + b.number;
                    break;
                }

                case SUB:
                {
                    DIFF_PARENT.value.number = a.number - b.number;
                    break;
                }
                
                case MUL:
                {
                    DIFF_PARENT.value.number = a.number * b.number;
                    break;
                }

                case DIV:
                {
                    if (cmpFloats(b.number, 0)) {
                        myDiffTree->error = ERROR_DIVISION_ON_ZERO;
                        printf ("ERROR: Division on zero\n");
                        
                        return myDiffTree;
                    }

                    DIFF_PARENT.value.number = a.number / b.number;
                    break;
                }

                case POW:
                {
                    DIFF_PARENT.value.number = pow (a.number, b.number);
                    break;
                }

                default:
                {
                    // myDiffTree->error = ERROR_UNKNOWN_OPERATION;
                    // printf ("UNKNOWN OPERATION IN TREE_CUT\n");
                    break;
                }

            }
            
        } else if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER && DIFF_PARENT.righty == 0) {
            union value c = {.number = 0.f};
            //DIFF_PARENT.value.number;
            c = TREE_DEL_LEFT

            if (!strcmp (DIFF_PARENT.value.operation, "ln")) {
                if (c.number > 0) {
                    DIFF_PARENT.value.number = log (c.number);
                } else {
                    printf ("Can't take ln from number less 0\n");
                }
            }

            if (!strcmp (DIFF_PARENT.value.operation, "sin")) {
                DIFF_PARENT.value.number = sin (c.number);
            }

            if (!strcmp (DIFF_PARENT.value.operation, "cos")) {
                DIFF_PARENT.value.number = cos (c.number);
            }

            if (!strcmp (DIFF_PARENT.value.operation, "tg")) {
                DIFF_PARENT.value.number = tan (c.number);
            }
        }

    switch (DIFF_PARENT.value.operation[0]) {
        case MUL:
        {
            if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER && myDiffTree->data[DIFF_PARENT.righty].type_of_value == VARIABLE) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 0)) {
                    TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = NUMBER;
                    DIFF_PARENT.value.number = 0;
                }

                if (cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 1)) {
                    TREE_DEL_LEFT
                    union value copy = TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            } else if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == VARIABLE && myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 0)) {
                    TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = NUMBER;
                    DIFF_PARENT.value.number = 0;
                } else if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 1)) {
                    union value copy = TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            }

            break;
        }

        case ADD:
        {
            if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER && myDiffTree->data[DIFF_PARENT.righty].type_of_value == VARIABLE) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 0)) {
                    TREE_DEL_LEFT
                    union value copy = TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            } else if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == VARIABLE && myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 0)) {
                    union value copy = TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            }

            break;
        }

        case DIV:
        {
            if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == VARIABLE && myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 1)) {
                    TREE_DEL_RIGHT
                    union value copy = TREE_DEL_LEFT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            }

            break;
        }

        case SUB:
        {
            if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == VARIABLE && myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 0)) {
                    TREE_DEL_RIGHT
                    union value copy = TREE_DEL_LEFT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                }
            }

            break;
        }

        case POW: 
        {
            if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER && myDiffTree->data[DIFF_PARENT.righty].type_of_value == VARIABLE) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 1)) {
                    TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = NUMBER;
                    DIFF_PARENT.value.number = 1;
                } else if (cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 0)) {
                    TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = NUMBER;
                    DIFF_PARENT.value.number = 0;
                }
            } else if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == VARIABLE && myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER) {
                if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 1)) {
                    union value copy = TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = VARIABLE;
                    DIFF_PARENT.value = copy;
                } else if (cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 0)) {
                    TREE_DEL_LEFT
                    TREE_DEL_RIGHT

                    myDiffTree->length -= 2;

                    DIFF_PARENT.type_of_value = NUMBER;
                    DIFF_PARENT.value.number = 1;
                }            
            }
            break;
        }

        default:
        {
            // printf ("parent:%d ", parent);
            // printf ("op:%s- ", DIFF_PARENT.value.operation);
            // myDiffTree->error = ERROR_UNKNOWN_OPERATION;
            // printf ("UNKNOWN OPERATION IN TREE_CUT\n");
            break;
        }
    };

    if (DIFF_PARENT.type_of_value == OPERATION && !strcmp (DIFF_PARENT.value.operation, "*")) {
        if (myDiffTree->data[DIFF_PARENT.lefty].type_of_value == NUMBER && cmpFloats(myDiffTree->data[DIFF_PARENT.lefty].value.number, 0)) {
            deleteSubTree (myDiffTree, DIFF_PARENT.lefty);
            deleteSubTree (myDiffTree, DIFF_PARENT.righty);
            DIFF_PARENT.type_of_value = NUMBER;
            DIFF_PARENT.value.number = 0;
            DIFF_PARENT.lefty = 0;
            DIFF_PARENT.righty = 0;
        }
        if (myDiffTree->data[DIFF_PARENT.righty].type_of_value == NUMBER && cmpFloats(myDiffTree->data[DIFF_PARENT.righty].value.number, 0)) {
            deleteSubTree (myDiffTree, DIFF_PARENT.lefty);
            deleteSubTree (myDiffTree, DIFF_PARENT.righty);
            DIFF_PARENT.type_of_value = NUMBER;
            DIFF_PARENT.value.number = 0;
            DIFF_PARENT.lefty = 0;
            DIFF_PARENT.righty = 0;
        }
    }
    }
    if (DIFF_PARENT.lefty != 0) {
        treeCut (myDiffTree, DIFF_PARENT.lefty);
    }

    if (DIFF_PARENT.righty != 0) {
        treeCut (myDiffTree, DIFF_PARENT.righty);
    }


    return myDiffTree;
}

void treeReduction (struct tree* myDiffTree) {
    struct tree myCTree = *myDiffTree;
    struct tree* myDiffCopy = &myCTree;
    while (!treeCmp (treeCut(myDiffTree), myDiffCopy)) {// while tree changes
        myCTree = *myDiffTree;
    }

    if (myDiffTree->length * 2 > myDiffTree->size) {
        treeResizeDown (myDiffTree);
    }
}

int treeCmp (struct tree* srcTree, struct tree* dstTree) {
    if (srcTree->length != dstTree->length) {
        return 0;
    }

    if (srcTree->free_node != dstTree->free_node) {
        return 0;
    }

    int size = srcTree->length;
    for (int i = 0; i < size; i++) {
        if (srcTree->data[i].lefty != dstTree->data[i].lefty) {
            return 0;
        }

        if (srcTree->data[i].righty != dstTree->data[i].righty) {
            return 0;
        }

        if (srcTree->data[i].type_of_value != dstTree->data[i].type_of_value) {
            return 0;
        }

        if (!cmpFloats (srcTree->data[i].value.number, dstTree->data[i].value.number)) {
            return 0;
        }

        if (srcTree->free[i] != dstTree->free[i]) {
            return 0;
        }
    }

    return 1;
}

union value treeDel (struct tree* myTree, int parent, int child) {
    union value result = {.number = -1};

    if (child == LEFTY) {
        TREE_DATA[PARENTLEFT].lefty = 0;
        TREE_DATA[PARENTLEFT].righty = 0;
        TREE_DATA[PARENTLEFT].type_of_value = FREE;

        result = TREE_DATA[PARENTLEFT].value;

        TREE_DATA[PARENTLEFT].value.number = POISON;
        myTree->free[PARENTLEFT] = myTree->free_node;
        myTree->free_node = PARENTLEFT;

        PARENTLEFT = 0;

        return result;
    } else if (child == RIGHTY) {
        TREE_DATA[PARENTRIGHT].lefty = 0;
        TREE_DATA[PARENTRIGHT].righty = 0;
        TREE_DATA[PARENTRIGHT].type_of_value = FREE;

        result = TREE_DATA[PARENTRIGHT].value;

        TREE_DATA[PARENTRIGHT].value.number = POISON;
        myTree->free[PARENTRIGHT] = myTree->free_node;
        myTree->free_node = PARENTLEFT;

        PARENTRIGHT = 0;

        return result;
    } else {
        myTree->error = ERROR_DELETE_UNKNOWN_CHILD;
        printf ("ERROR: OMG... No so much children in node!");

        return result;
    }
}

void deleteSubTree (struct tree* myTree, int parent) {
    if (PARENTLEFT != 0) {
        deleteSubTree (myTree, PARENTLEFT);
    }

    if (PARENTRIGHT != 0) {
        deleteSubTree (myTree, PARENTRIGHT);
    }
    
    treeDelCurr (myTree, parent);
}

void treeDelCurr (struct tree* myTree, int parent) {
    TREE_DATA[parent].value = {.number = POISON};
    TREE_DATA[parent].type_of_value = FREE;
    PARENTLEFT = 0;
    PARENTRIGHT = 0;

    myTree->free[parent] = myTree->free_node;
    myTree->free_node = parent;

    myTree->length--;
}

int cmpFloats (float a, float b) {
    if (abs(a - b) < EPSILON) {
        return 1;
    }

    return 0;
}

void writeTex (struct tree* myTree, int current, FILE* f) {
    if (TREE_DATA[current].type_of_value == OPERATION) {
        if (!strcmp (TREE_DATA[current].value.operation, "sin")) {
            fprintf(f, "\\sin(");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf (f, ")");
        } else if (!strcmp (TREE_DATA[current].value.operation, "cos")) {
            fprintf(f, "\\cos(");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf (f, ")");
        } else if (!strcmp (TREE_DATA[current].value.operation, "tg")) {
            fprintf(f, "\\tg(");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf (f, ")");
        } else if (!strcmp (TREE_DATA[current].value.operation, "ln")) {
            fprintf(f, "\\ln(");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf (f, ")");
        } else if (!strcmp (TREE_DATA[current].value.operation, "/")) {
            fprintf(f, "\\frac{");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf (f, "}");
            
            fprintf(f, "{");
            writeTex (myTree, TREE_DATA[current].righty, f);
            fprintf (f, "}");
        } else if (!strcmp (TREE_DATA[current].value.operation, "+")) {
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf(f, " + ");
            writeTex (myTree, TREE_DATA[current].righty, f);
        } else if (!strcmp (TREE_DATA[current].value.operation, "-")) {
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf(f, " - ");
            writeTex (myTree, TREE_DATA[current].righty, f);
        } else if (!strcmp (TREE_DATA[current].value.operation, "*")) {
            fprintf(f, "(");
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf(f, ") \\cdot (");
            writeTex (myTree, TREE_DATA[current].righty, f);
            fprintf(f, ")");
        } else if (!strcmp (TREE_DATA[current].value.operation, "^")) {
            writeTex (myTree, TREE_DATA[current].lefty, f);
            fprintf(f, "^{");
            writeTex (myTree, TREE_DATA[current].righty, f);
            fprintf(f, "}");
        }
    } else if (TREE_DATA[current].type_of_value == VARIABLE) {
        fprintf (f, "%s", TREE_DATA[current].value.variable);
    } else if (TREE_DATA[current].type_of_value == NUMBER) {
        fprintf (f, "%f", TREE_DATA[current].value.number);
    }
}
