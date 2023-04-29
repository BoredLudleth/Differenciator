#ifndef FLASH_CODE_HPP
#define FLASH_CODE_HPP

#define DL diffNode(myDiffTree, myTree, &(myTree->data[n->lefty]), dVar, parent);

#define DR diffNode(myDiffTree, myTree, &(myTree->data[n->righty]), dVar, parent);

#define PARENTLEFT myTree->data[parent].lefty

#define PARENTRIGHT myTree->data[parent].righty

#define ADD_OPERATION treeAdd(myDiffTree,parent,n->value,OPERATION);

#define ADD_MUL treeAdd(myDiffTree,parent,MulValue,OPERATION);

#endif