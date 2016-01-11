#include "global.h"

TreeNode::TreeNode(int line, NodeKind kind){
    this->line = line;
    this->kind = kind;
    this->child = NULL;
    this->sibling = NULL;
}

TreeNode::~TreeNode(){
    if (child != NULL) delete child;
    if (sibling != NULL) delete sibling;
}
