#include "bintree.hpp"
#include "differenciator.hpp"

void treeCtor (struct tree* myTree, int type, union value value, FILE* log_file) {
    static int count_tree = 0;

    if (log_file == nullptr) {
        char log_name[15] = "log";
        char number_of_log[5] = "";
        strcat (log_name, inttoa(count_tree, number_of_log));
        strcat (log_name, ".html");

        myTree->log_file = fopen (log_name, "w+");

        count_tree++;
    } else {
        myTree->log_file = log_file;
    }

    fprintf (myTree->log_file, "<pre>\n");
    
    fprintf (myTree->log_file, "<h2>Tree Ctor started</h2>\n");

    myTree->size = 10;

    myTree->data = (node*) calloc (myTree->size, sizeof (node));
    myTree->free = (int*) calloc (myTree->size, sizeof (int));

    myTree->free[0] = -1;
    myTree->free[1] = -1;
    myTree->free_node = 1;

    if (type == OPERATION) {
        myTree->data[myTree->free_node].value.operation = value.operation; 
    } else  if (type == NUMBER){
        myTree->data[myTree->free_node].value.number = value.number;
    } else if (type == VARIABLE) {
        myTree->data[myTree->free_node].value = value;
    }

    myTree->data[1].type_of_value  = type;

    for (int i = 2; i < myTree->size - 1; i++) {
        myTree->free[i] = i + 1;
        myTree->data[i].type_of_value = FREE;
    }

    myTree->length = 1;

    myTree->free_node = 2;

    myTree->free[myTree->size - 1] = 0;

    myTree->head = myTree->data;

    fprintf (myTree->log_file, "Tree Ctor complete\n");
}

void treeDtor (struct tree* myTree) {
    for (int i = 0; i < myTree->size; i++) {
        myTree->data[i].value.number = POISON;
        myTree->data[i].lefty = POISON;
        myTree->data[i].righty = POISON;
    }

    for (int i = 0; i < myTree->size; i++) {
        myTree->free[i] = POISON;
    }

    free (myTree->free);
    myTree->free = nullptr;

    free (myTree->data);
    myTree->data = nullptr;


    myTree->error = 0;
    myTree->free_node = 0;
    myTree->head = nullptr;
    myTree->size = 0;

    fprintf (myTree->log_file, "<h2>Tree destroyed</h2>\n");
    fprintf (myTree->log_file, "Тут могла быть ваша Полторашка\n");
}

void treeResizeUp (struct tree* myTree) {
        fprintf (myTree->log_file, "Tree resizing..\n");
        myTree->free_node = myTree->size;
        myTree->size *= 2;
        myTree->data = (node*) realloc (myTree->data, myTree->size * sizeof(node));

        if (myTree->data == NULL) {
            myTree->error = ERROR_RESIZE;
            printf ("ERROR: Resizing incomplete\n");
            return;
        }

        myTree->head = myTree->data;

        myTree->free = (int*) realloc (myTree->free, myTree->size * sizeof(int));

        for (int i = (myTree->size / 2); i < (myTree->size - 1); i++) {
            myTree->free[i] = i + 1;
            myTree->data[i].value.number = POISON;
            myTree->data[i].lefty = 0;
            myTree->data[i].righty = 0;
            myTree->data[i].type_of_value = FREE;
        }

        myTree->data[myTree->size - 1].type_of_value = FREE;
        myTree->data[myTree->size - 1].value.number = POISON;
        myTree->data[myTree->size - 1].lefty = 0;
        myTree->data[myTree->size - 1].righty = 0;
        myTree->free[myTree->size - 1] = 0;
}

void treeResizeDown (struct tree* myTree) {
    struct tree reallocTree;
    struct tree* copyTree = &reallocTree;

    treeCtor (copyTree, myTree->data[1].type_of_value, myTree->data[1].value);

    if (myTree->data[1].lefty != 0) {
        treeCopy (copyTree, myTree, &(myTree->data[myTree->data[1].lefty]), HEAD);
    }
    if (myTree->data[1].righty != 0) {
        treeCopy (copyTree, myTree, &(myTree->data[myTree->data[1].righty]), HEAD);
    }
    
    treeDtor (myTree);

    *myTree = reallocTree;

    fprintf (myTree->log_file, "Tree resized down\n");
}

int treeAdd (struct tree* myTree, int parent, union value value, int type) {
    fprintf (myTree->log_file, "Adding new element to the tree\n");

    if (myTree->free[parent] != -1) {
        myTree->error = ERROR_UNDEFENITE_PARENT;
        printf ("ERROR: Can't add parent is secret for us, value did't added\n");
        fprintf (myTree->log_file, "ERROR: Can't add parent is secret for us, value did't added\n");

        return -1;
    }

    if (myTree->data[parent].type_of_value == NUMBER) {
        myTree->error = ERROR_PARENT_IS_NUMBER;
        printf ("We didn't add your value in tree, because the parent is number!\n");
        fprintf (myTree->log_file, "We didn't add your value in tree, because the parent is number!\n");

        return -1;
    }

    if (myTree->data[parent].type_of_value == VARIABLE) {
        myTree->error = ERROR_PARENT_IS_VARIABLE;
        printf ("We didn't add your value in tree, because the parent is variable!\n");
        fprintf (myTree->log_file, "We didn't add your value in tree, because the parent is variable!\n");

        return -1;
    }

    if (myTree->free_node == 0) {
        treeResizeUp(myTree);
    }

    if (parent > myTree->size) {
        myTree->error = ERROR_PARENT_OUT_OF_RANGE;
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
                myTree->data[myTree->free_node].value = value;
                printf ("Added in left: %s\n", value.variable);
                fprintf (myTree->log_file, "Added number in left: %s\n", value.variable);
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
                myTree->data[myTree->free_node].value = value;
                printf ("Added in right: %s\n", value.variable);
                fprintf (myTree->log_file, "Added number in right: %s\n", value.variable);
            }

            myTree->data[myTree->free_node].type_of_value  = type;
            
            myTree->free_node = myTree->free[current];
            myTree->free[current] = -1;

            return current;
        } else {
            myTree->error = ERROR_ALREADY_TWO_CHILDREN;
            printf ("ERROR: Parent has two children, value did't added\n");
            fprintf (myTree->log_file, "ERROR: Parent has two children, value did't added\n");

            return -1;
        }
    }
    return -1;
}

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

    num++;

    FILE* output = fopen (name_of_file,"w+");

    myTree->output = output;
    if  (myTree->output == nullptr) {
        myTree->error = ERROR_GRAPHVIZ_FILE_NOT_OPEN;
        printf ("File didn't open\n");
        return;
    }

    fprintf (myTree->output, "digraph TB\n{\n");
    fprintf (myTree->output, "node [shape=record fontname=Arial];\n");
    fprintf (myTree->output, "edge [dir=both, style=\"\", constraint=false, color = darkred]");
    fprintf (myTree->output, "rankdir = TB;\n");
    fprintf (myTree->output, "splines = \"ortho\"\n");
    
    fprintf (myTree->output, "head [label = \"HEAD\", style = filled, fillcolor = \"#d01234\"];\n");

    for (int i = 1; i < myTree->size; i++) {
        if (myTree->data[i].type_of_value == NUMBER) {
            fprintf (output, "node%d [label = \"{<f1> value = %f|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d| <f6> num}\", shape=record, style = filled, fillcolor = \"#d1234f\"];\n",
                    i, myTree->data[i].value.number, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);     
        } else if (myTree->data[i].type_of_value == OPERATION) {
            fprintf (output, "node%d [label = \"{<f1> value = %c|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d| <f6>op}\", shape=record, style = filled, fillcolor = \"#d0ffff\"];\n",
                    i, myTree->data[i].value.operation, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);   
        } else if (myTree->data[i].type_of_value == VARIABLE) {
            fprintf (output, "node%d [label = \"{<f1> value = %s|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d| <f6> var}\", shape=record, style = filled, fillcolor = \"#00cf00\"];\n",
                    i, myTree->data[i].value.variable, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);    
        } else if (myTree->data[i].type_of_value == FREE) {
            fprintf (output, "node%d [label = \"{<f1> value = %f|  <f2> addr = %d| {<f3> left = %d |<f4> right = %d}| <f5> free = %d| <f6> free}\", shape=record, style = filled, fillcolor = \"#ffffaf\"];\n",
                    i, myTree->data[i].value.number, i, myTree->data[i].lefty, myTree->data[i].righty, myTree->free[i]);  

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

    compileDot (name_of_file, pic_name);

    fprintf (myTree->log_file, "<img src = img/%s width 40/>\n\n\n", pic_name);
}

void compileDot (char* name_of_file, char* pic_name) {
    char command[50] = "dot ";

    strcat (command, name_of_file);
    strcat (command, " -Tpng -o ");
    strcat (command,"img/");
    strcat (command, pic_name);

    system(command);
}

void tree_check (struct tree* myTree) {
    if (myTree->error != 0) {
        exit(1);
    }
}
