#include "bintree.hpp"

void treeCtor (struct tree* myTree, int type, union value value) {
    myTree->log_file = fopen ("log.txt", "w+");
    fprintf (myTree->log_file, "LOG-file started\n");
    fprintf (myTree->log_file, "Tree Ctor started\n");

    myTree->size = 10;

    myTree->data = (node*) calloc (myTree->size, sizeof (node));
    myTree->free = (int*) calloc (myTree->size, sizeof (int));
    // myTree->varLabels = (char**) calloc (myTree->numberOfLabels, sizeof (char*));

    myTree->free[0] = -1;
    myTree->free[1] = -1;
    myTree->free_node = 1;

    if (type == OPERATION) {
        myTree->data[myTree->free_node].value.operation = value.operation; 
    } else  if (type == NUMBER){
        myTree->data[myTree->free_node].value.number = value.number;
    } else if (type == VARIABLE) {
        myTree->data[myTree->free_node].value.variable = value.variable;
    }

    myTree->data[1].type_of_value  = type;

    for (int i = 2; i < myTree->size - 1; i++) {
        myTree->free[i] = i + 1;
    }

    myTree->free_node = 2;

    myTree->free[myTree->size - 1] = 0;

    myTree->head = myTree->data;

    fprintf (myTree->log_file, "Tree Ctor complete\n");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void treeDtor (struct tree* myTree) {
    for (int i = 0; i < myTree->size; i++) {
        myTree->data[i].value.number = POISON;
        myTree->data[i].lefty = POISON;
        myTree->data[i].righty = POISON;
    }

    for (int i = 0; i < myTree->size; i++) {
        myTree->free[i] = POISON;
    }
DBG
    free (myTree->free);
    myTree->free = nullptr;
DBG
    free (myTree->data);
    myTree->data = nullptr;


    myTree->error = 0;      //make enum for errors
    myTree->free_node = 0;
    myTree->head = nullptr;
    myTree->size = 0;

    fprintf (myTree->log_file, "Tree destroyed\n");
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void treeResize (struct tree* myTree) {
        fprintf (myTree->log_file, "Tree resizing..\n");                  //what if resizing couldn't be complete...
        myTree->free_node = myTree->size;
        myTree->size *= 2;
        myTree->data = (node*) realloc (myTree->data, myTree->size * sizeof(node)); // check can make break
        myTree->head = myTree->data;

        myTree->free = (int*) realloc (myTree->free, myTree->size * sizeof(int));

        for (int i = (myTree->size / 2); i < (myTree->size - 1); i++) {
            DBG
            myTree->free[i] = i + 1;
            myTree->data[i].value.number = POISON;
            myTree->data[i].lefty = 0;
            myTree->data[i].righty = 0;
        }

        myTree->data[myTree->size - 1].value.number = POISON;
        myTree->data[myTree->size - 1].lefty = 0;
        myTree->data[myTree->size - 1].righty = 0;
        myTree->free[myTree->size - 1] = 0;
}



int treeAdd (struct tree* myTree, int parent, union value value, int type) {
    fprintf (myTree->log_file, "Adding new element to the tree\n");

    if (myTree->free[parent] != -1) {
        DBG
        printf ("ERROR: Can't add parent is secret for us, value did't added\n");
        fprintf (myTree->log_file, "ERROR: Can't add parent is secret for us, value did't added\n");

        return -1;
    }

    if (myTree->data[parent].type_of_value == NUMBER) {
        printf ("We didn't add your value in tree, because the parent is number!\n");
        fprintf (myTree->log_file, "We didn't add your value in tree, because the parent is number!\n");

        return -1;
    }

    if (myTree->data[parent].type_of_value == VARIABLE) {
        printf ("We didn't add your value in tree, because the parent is variable!\n");
        fprintf (myTree->log_file, "We didn't add your value in tree, because the parent is variable!\n");

        return -1;
    }

    if (myTree->free_node == 0) {
        DBG

        treeResize(myTree);
    }

    if (parent > myTree->size) {
        DBG
        printf ("ERROR: Parent > tree size, value did't added\n");

        return -1;
    }
    else
    {
        int current = myTree->free_node;

        if (myTree->data[parent].lefty == 0) {
            myTree->data[parent].lefty = myTree->free_node;
            
            if (type == OPERATION) {
                myTree->data[myTree->free_node].value.operation = value.operation;
                printf ("Added in left: %d\n", value.operation);
                fprintf (myTree->log_file, "Added operation  %c in left\n", value.operation);
            } else  if (type == NUMBER){
                myTree->data[myTree->free_node].value.number  = value.number;
                printf ("Added in left: %f\n", value.number);
                fprintf (myTree->log_file, "Added number in left: %f\n", value.number);
            } else if (type == VARIABLE) {
                myTree->data[myTree->free_node].value.variable = value.variable;
                printf ("Added in left: %c\n", value.variable);
                fprintf (myTree->log_file, "Added number in left: %c\n", value.variable);
            }

            myTree->data[myTree->free_node].type_of_value  = type;

            myTree->free_node = myTree->free[current];
            myTree->free[current] = -1;

            return current;
        } else  if (myTree->data[parent].righty == 0) {
            myTree->data[parent].righty = myTree->free_node;
            
            if (type == OPERATION) {
                myTree->data[myTree->free_node].value.operation = value.operation; 
                printf ("Added in right: %d\n", value.operation);
                fprintf (myTree->log_file, "Added operation  %c in right\n", value.operation);
            } else  if (type == NUMBER){
                myTree->data[myTree->free_node].value.number  = value.number;
                printf ("Added in right: %f\n", value.number);
                fprintf (myTree->log_file, "Added number in right: %f\n", value.number);
            } else if (type == VARIABLE) {
                myTree->data[myTree->free_node].value.variable = value.variable;
                printf ("Added in right: %c\n", value.variable);
                fprintf (myTree->log_file, "Added number in right: %c\n", value.variable);
            }

            myTree->data[myTree->free_node].type_of_value  = type;
            
            myTree->free_node = myTree->free[current];
            myTree->free[current] = -1;

            return current;
        } else {
            DBG
            printf ("ERROR: Parent has two children, value did't added\n");
            fprintf (myTree->log_file, "ERROR: Parent has two children, value did't added\n");

            return -1;
        }
    }
    return -1;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

char* inttoa(int n, char* s) {
    int i = 0;
    int sign = 0;
 
    if ((sign = n) < 0)
        n = -n;
    i = 0;
    do {
        s[i++] = (char) (n % 10) + '0';
    } while ((n /= 10) > 0);    
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);

    return s;
}

void reverse(char* s) {
    int i = 0, j = 0;
    char c = ' ';

    for (i = 0, j = (int) strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

void graph_dump (struct tree* myTree) {
    fprintf (myTree->log_file, "Graph dump!\n");

    static int num = 0;
    char name_of_file[20] = "txt/image";
    char im_num[5] = {};
    strcat (name_of_file, inttoa(num, im_num));
    strcat (name_of_file, ".txt\0");

    char pic_name[20] = "image";
    strcat (pic_name, im_num);
    strcat (pic_name, ".png\0");
DBG
    num++;

    FILE* output = fopen (name_of_file,"w+");
    myTree->output = output;
    if  (myTree->output == nullptr) {
        printf ("File didn't open\n");
    }
DBG
    fprintf (myTree->output, "digraph TB\n{\n");
    fprintf (myTree->output, "node [shape=record fontname=Arial];\n");
    fprintf (myTree->output, "edge [dir=both, style=\"\", constraint=false, color = darkred]");
    fprintf (myTree->output, "rankdir = TB;\n");
    fprintf (myTree->output, "splines = \"ortho\"\n");
    
    fprintf (myTree->output, "head [label = \"HEAD\", style = filled, fillcolor = \"#d01234\"];\n");
DBG
    for (int i = 1; i < myTree->size; i++) {
        if (myTree->data[i].type_of_value == NUMBER) {
            fprintf (output, "node%d [label = \"{<f1> value = %d|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d}\", shape=record, style = filled, fillcolor = \"#d0ffff\"];\n",
                    i, myTree->data[i].value.number, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);     //integer add  
        } else if (myTree->data[i].type_of_value == OPERATION) {
            fprintf (output, "node%d [label = \"{<f1> value = %c|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d}\", shape=record, style = filled, fillcolor = \"#d0ffff\"];\n",
                    i, myTree->data[i].value.operation, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);     //integer add  
 
        }else if (myTree->data[i].type_of_value == VARIABLE) {
            fprintf (output, "node%d [label = \"{<f1> value = %c|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d}\", shape=record, style = filled, fillcolor = \"#d0ffff\"];\n",
                    i, myTree->data[i].value.variable, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);     //integer add  
        }
    }

    for (int i = 1; i < myTree->size; i++) {
        if (myTree->data[i].lefty != 0)  {
            fprintf (myTree->output, "node%d -> node%d[dir=both, style=filled, constraint=true];\n", i, myTree->data[i].lefty);
        }

        if (myTree->data[i].righty != 0)  {
            fprintf (myTree->output, "node%d -> node%d[dir=both, style=filled, constraint=true];\n", i, myTree->data[i].righty); 
        }
    }

    fprintf (myTree->output, "}\n");
    fclose (myTree->output);
    myTree->output = nullptr;
DBG
    char command[50] = "dot ";
    strcat (command, name_of_file);
    strcat (command, " -Tpng -o ");
    strcat (command,"img/");
    strcat (command, pic_name);
DBG
    system(command);
DBG
}
