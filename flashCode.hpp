#ifndef FLASH_CODE_HPP
#define FLASH_CODE_HPP

#define TREE_DATA myTree->data

#define DIFF_PARENT myDiffTree->data[parent]

#define DL diffNode(myDiffTree, myTree, &(TREE_DATA[n->lefty]), dVar, parent);

#define DR diffNode(myDiffTree, myTree, &(TREE_DATA[n->righty]), dVar, parent);

#define PARENTLEFT TREE_DATA[parent].lefty

#define PARENTRIGHT TREE_DATA[parent].righty

#define ADD_OPERATION treeAdd(myDiffTree,parent,n->value,OPERATION);

#define TREE_MUL treeAdd(myDiffTree,parent,MulValue,OPERATION);

#define TREE_ADD treeAdd(myDiffTree,parent,PlusValue,OPERATION);

#define TREE_SUB treeAdd(myDiffTree,parent,SubValue,OPERATION);

#define TREE_DIV treeAdd(myDiffTree,parent,DivValue,OPERATION);

#define TREE_DEL_LEFT treeDel(myDiffTree,parent,LEFTY);

#define TREE_DEL_RIGHT treeDel(myDiffTree,parent,RIGHTY);



#endif