#include "global.h"

TreeNode::TreeNode(int line, NodeKind kind, TreeNode* parent){
    this->line = line;
    this->kind = kind;
    this->child = NULL;
    this->sibling = NULL;
	this->parent = parent;
}

TreeNode::~TreeNode(){
    if (child != NULL) delete child;
    if (sibling != NULL) delete sibling;
}
