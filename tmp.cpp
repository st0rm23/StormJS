TreeNode* sy(){
    TreeNode* t = new TreeNode(scanner->getLine, );
    t->value = token;
    match();
    return t;
}


TreeNode* sy(){
    TreeNode* t = new TreeNode(scanner->getLine, );
    TreeNode* c = NULL;
    c = t->child = sy();
    c = c->sibling = sy();
    return t;
}
