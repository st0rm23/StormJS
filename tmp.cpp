TreeNode* Parser::sy(){
    TreeNode* t = new TreeNode(scanner->getLine(), );
    t->value = token;
    match();
    return t;
}


TreeNode* Parser::sy(){
    TreeNode* t = new TreeNode(scanner->getLine(), );
    TreeNode* c = NULL;
    c = t->child = sy();
    c = c->sibling = sy();
    return t;
}

Variable opAssign(const Variable *a, TokenType op, const Variable *b){
    switch (op){

        default :
            break;
    }
}


switch (var.type) {
    case INTEGER:
        break;
    case DECIMAL:
        break;
    case HEX:
        break;
    case STRING:
        break;
    case CHARACTER:
        break;
    case VBOOLEAN:
        break;
    case EMPTY:
        break;
    default:
        break;
}
