#include "global.h"

void Execution::testNode(TreeNode* t, NodeKind kind, bool needChild){
    if (t == NULL) throw ExecutionException();
    if (t->kind != kind) throw ExecutionException();
    if (needChild && t->child == NULL) throw ExecutionException();
}


string Execution::exIdentifier(TreeNode* t){
    this->testNode(t, Identifier, false);
    return t->token->value;
}

string Execution::exAssignmentOperator(TreeNode* t){
    this->testNode(t, AssignmentOperator, false);
    return t->token->value;
}

void Execution::exProgram(TreeNode* t){
    this->testNode(t, Program, true);
    TreeNode* c = t->child;
    this->exSourceElements(c);
}

void Execution::exSourceElements(TreeNode* t){
    this->testNode(t, SourceElements, true);
    TreeNode* c = t->child;
    this->exSourceElement(c);
    while ((c = c->sibling) != NULL)
        this->exSourceElement(c);
    return t;
}

void Execution::exSourceElement(TreeNode* t){
    this->testNode(t, SourceElement, true);
    TreeNode* c = t->child;
    if (c->kind == FunctionDeclaration)
        this->exFunctionDeclaration(c);
    else
        this->exStatement(c);
}

void Execution::exFunctionDeclaration(TreeNode* t){
    this->testNode(t, FunctionDeclaration, true);
    TreeNode* c = t->child;
    if (c->kind != Identifier) throw ExecutionException();
    string funcId = exIdentifier();
    this->nowST->insert(funcId, t);
}

void Execution::exStatement(TreeNode* t){   //incomplete
    this->testNode(t, Statement, true);
    TreeNode* c = t->child;
    switch (c->token.type){
        case VAR:
            this->exVariableStatement(c);
            break;
        default:
            c = t->child = exExpressionStatement(c);
            break;
    }
}

void Execution::exVariableStatement(TreeNode* t){
    this->testNode(t, VariableStatement, true);
    TreeNode* c = t->child;
    this->exVariableDeclaration(c);
}

void Execution::exVariableDeclarationList(TreeNode* t){
    this->testNode(t, VariableDeclarationList, true);
    TreeNode* c = t->child;
    this->exVariableDeclaration(c);
    while ((c = c->sibling) != NULL)
        this->exVariableDeclaration(c);
}

void Execution::exVariableDeclaration(TreeNode* t){
    this->testNode(t, VariableDeclaration, true);
    TreeNode* c = t->child;
    string id = exIdentifier(c);
    Variable* var = NULL;
    c = c->sibling;
    if (c != NULL) var = this->exInitialiser(c);
    else var = new VarUndefined();
    this->nowAR->setVar(exIdentifier(c), var);
    var->release();
}

Variable* Execution::exInitialiser(TreeNode* t){
    this->testNode(t, Initialiser, true);
    TreeNode* c = t->child;
    return this->exAssignmentExpression(c);
}

Variable* Execution::exAssignmentExpression(TreeNode* t){       //AssignmentExpression -> ( LeftHandSideExpression AssignmentOperator AssignmentExpression ) | ( ConditionalExpression )
    this->testNode(t, AssignmentExpression, true);
    TreeNode* c = t->child;
    if (c->kind == LeftHandSideExpression){
        TreeNode* leftNode = c;
        TreeNode* opNode = c->sibling;
        TreeNode* rightNode = c->sibling->sibling;

        Variable*& left = exLeftHandSideExpression(leftNode);
        Variable* right = exAssignmentExpression(rightNode);
        string op = exAssignmentOperator(opNode);
        Variable* res = Expression::opAssign(&left, op, right);

        left->release();
        right->release();
        return res;
    }
    else {
        Variable* res = ConditionalExpression(c);
        return res;
    }
}

Variable* Execution::exConditionalExpression(TreeNode* t){
    this->testNode(t, ConditionalExpression, true);
    TreeNode* c = t->child;

    if (c->sibling != NULL){
        TreeNode* a = c;
        TreeNode* b = c->sibling;
        c = c->sibling->sibling;
        Variable *ta = exLogicalORExpression(a);
        Variable *tb = exAssignmentExpression(b);
        Variable *tc = exAssignmentExpression(c);
        Variable *res = Expression::opCondition(ta, tb, tc);
        ta->release();
        tb->release();
        tc->release();
        return res;
    } else{
        return exLogicalORExpression(c);
    }
}

Variable* Execution::exLogicalORExpression(TreeNode* t){
    this->testNode(t, LogicalORExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exLogicalANDExpression(c);
        while (c->sibling != NULL){
            string op = exLogicalOROperator(c->sibling);
            Variable *b = exLogicalANDExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opLogical(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exLogicalANDExpression(c);
    }
}

string Execution::exLogicalOROperator(TreeNode* t){
    this->testNode(t, LogicalOROperator, false);
    return t->token->value;
}


Variable* Execution::exLogicalANDExpression(TreeNode* t){
    this->testNode(t, LogicalANDExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseORExpression(c);
        while (c->sibling != NULL){
            string op = exLogicalANDOperator(c->sibling);
            Variable *b = exBitwiseORExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opLogical(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exBitwiseORExpression(c);
    }
}

string Execution::exLogicalANDOperator(TreeNode* t){
    this->testNode(t, LogicalANDOperator, false);
    return t->token->value;
}



Variable* Execution::exBitwiseORExpression(TreeNode* t){
    this->testNode(t, BitwiseORExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseXORExpression(c);
        while (c->sibling != NULL){
            string op = exBitwiseOROperator(c->sibling);
            Variable *b = exBitwiseXORExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opBitwise(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exBitwiseXORExpression(c);
    }
}

string Execution::exBitwiseOROperator(TreeNode* t){
    this->testNode(t, BitwiseOROperator, false);
    return t->token->value;
}

Variable* Execution::exBitwiseXORExpression(TreeNode* t){
    this->testNode(t, BitwiseXORExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseANDExpression(c);
        while (c->sibling != NULL){
            string op = exBitwiseXOROperator(c->sibling);
            Variable *b = exBitwiseANDExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opBitwise(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exBitwiseANDExpression(c);
    }
}

string Execution::exBitwiseXOROperator(TreeNode* t){
    this->testNode(t, BitwiseXOROperator, false);
    return t->token->value;
}


Variable* Execution::exBitwiseANDExpression(TreeNode* t){
    this->testNode(t, BitwiseANDExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exEqualityExpression(c);
        while (c->sibling != NULL){
            string op = exBitwiseANDOperator(c->sibling);
            Variable *b = exEqualityExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opBitwise(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exEqualityExpression(c);
    }
}

string Execution::exBitwiseANDOperator(TreeNode* t){
    this->testNode(t, BitwiseANDOperator, false);
    return t->token->value;
}

Variable* Execution::exEqualityExpression(TreeNode* t){
    this->testNode(t, BitwiseANDExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exRelationalExpression(c);
        while (c->sibling != NULL){
            string op = exEqualityOperator(c->sibling);
            Variable *b = exRelationalExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opEqual(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exRelationalExpression(c);
    }
}

string Execution::exEqualityOperator(TreeNode* t){
    this->testNode(t, EqualityOperator, false);
    return t->token->value;
}

Variable* Execution::exRelationalExpression(TreeNode* t){
    this->testNode(t, RelationalExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exShiftExpression(c);
        while (c->sibling != NULL){
            string op = exEqualityOperator(c->sibling);
            Variable *b = exShiftExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opRelation(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exShiftExpression(c);
    }
}

string Execution::exRelationalOperator(TreeNode* t){
    this->testNode(t, RelationalOperator, false);
    return t->token->value;
}

Variable* Execution::exShiftExpression(TreeNode* t){
    this->testNode(t, ShiftExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exAdditiveExpression(c);
        while (c->sibling != NULL){
            string op = exShiftOperator(c->sibling);
            Variable *b = exAdditiveExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opShift(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exAdditiveExpression(c);
    }
}

string Execution::exShiftOperator(TreeNode* t){
    this->testNode(t, ShiftOperator, false);
    return t->token->value;
}

Variable* Execution::exAdditiveExpression(TreeNode* t){
    this->testNode(t, AdditiveExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exMultiplicativeExpression(c);
        while (c->sibling != NULL){
            string op = exAdditiveOperator(c->sibling);
            Variable *b = exMultiplicativeExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opAdditive(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exMultiplicativeExpression(c);
    }
}

string Execution::exAdditiveOperator(TreeNode* t){
    this->testNode(t, AdditiveOperator, false);
    return t->token->value;
}

Variable* Execution::exMultiplicativeExpression(TreeNode* t){
    this->testNode(t, MultiplicativeExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exUnaryExpression(c);
        while (c->sibling != NULL){
            string op = exMultiplicativeOperator(c->sibling);
            Variable *b = exUnaryExpression(c->sibling->sibling);
            c = c->sibling->sibling;

            Variable *tmp = a;
            a = Expression::opMultiplicative(a, op, b);
            tmp->release();
            b->release();
        }
        return a;
    }
    else {
        return exUnaryExpression(c);
    }
}


string Execution::exMultiplicativeOperator(TreeNode* t){
    this->testNode(t, MultiplicativeOperator, false);
    return t->token->value;
}


Variable*  Execution::exUnaryExpression(TreeNode* t){
    this->testNode(t, UnaryExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        string op = exUnaryOperator(c);
        Variable* tmp = UnaryExpression(c->sibling);
        Variable* res = Expression::opUnary(op, tmp);
        tmp->release();
        return res;
    } else{
        return PostfixExpression(c);
    }
}


string  Execution::exUnaryOperator(TreeNode* t){
    this->testNode(t, UnaryOperator, false);
    return t->token->value;
}

Variable* Execution::exPostfixExpression(TreeNode* t){
    this->testNode(t, PostfixExpression, true);
    TreeNode* c = t->child;

    Variable*& res = exLeftHandSideExpression(c);
    if (c->sibling != NULL){
        string op = exPostfixOperator(c);
        Variable* a = Expression::opPostfix(op, res);
        res->release();
        return a;
    }
    else return res;
}

string Execution::exPostfixOperator(TreeNode* t){
    this->testNode(t, PostfixOperator, false);
    return t->token->value;
}


Variable*& Execution::exLeftHandSideExpression(TreeNode* t){      //LeftHandSideExpression -> MemberExpression [ Arguments ( CallExpressionPart )* ];
    this->testNode(t, LeftHandSideExpression, true);
    TreeNode* c = t->child;

    Variable*& res = exMemberExpression(c);         //incomplete
    if (c->sibling != NULL){        //it's a calling
        if (res->type != Varialbe::FUNCTION)
            throw ExecutionException();

        vector<Variable*>& vec = exArguments(c->sibling);
        VarFunction *&vf = (VarFunction*)res;
        if (vf->ar == NULL) {
            vf->ar = new ActiveRecord()
            TreeNode* p = t;
            p = p->parent;
            while (p != NULL){
                if (p->kind == FunctionDeclaration || p->kind == Program) break;
                p = p->parent;
            }
            ActiveRecord* ar = nowAR;
            while (ar != NULL){
                if (ar->node == p) break;
                ar = ar->controlLink;
            }
            ActiveRecord* accessLink = ar;
            vf->ar = new ActiveRecord(nowAR, accessLink, t);
        }
        Variable*& var = runFunction(vf, vec);
        res->release();
        return var;
    }
    else return res;
}

vector<Variable*>& Execution::exArguments(TreeNode* t){
    this->testNode(t, Arguments, false);
    TreeNode* c = t->child;
    if (c == NULL) {
        vector<Variable*> res;
        return res;
    } else {
        return exArgumentList();
    }
}

TreeNode* Execution::exArgumentList(TreeNode* t){
    this->testNode(t, LeftHandSideExpression, true);
    TreeNode* c = t->child;
    vector<Variable *> res;
    res.push_back(exAssignmentExpression(c));
    c = c->sibling;
    while (c != NULL){
        res.push_back(exAssignmentExpression(c));
        c = c->sibling;
    }
    return res;
}


Variable*& Execution::exMemberExpression(TreeNode* t){
    this->testNode(t, MemberExpression, true);
    TreeNode* c = t->child;
    Variable*& res = exPrimaryExpression(c);         //incomplete
    return res;
}

Variable*& Execution::exPrimaryExpression(TreeNode* t){
    this->testNode(t, PrimaryExpression, true);     //incomplete
    TreeNode* c = t->child;

    if (c->kind == Identifier){
        string id = exIdentifier(c);
        if (this->ST->fetchSymbol(t, id) != NULL){
            TreeNode *node = this->ST->fetchSymbol(t, id);
            VarFunction*& var = new VarFunction();
            var->value = node;
            return var;
        } else if (this->nowAR->getVar(id) != NULL) {
            return this->nowAR->getVar(id);
        } else throw ExecutionException();
    } else if (c->kind == Literal){
        return exLiteral(c);
    } else return exExpression(c);
}

Variable*& Execution::exLiteral(TreeNode* t){
    this->testNode(t, Literal, false);
    Variable *& res = Variable::variableFromTreeNode(t);
    return res;
}


vector<string>& Execution::exFormalParameterList(TreeNode* t){
    this->testNode(t, FormalParameterList, true);     //incomplete
    TreeNode* c = t->child;
    vector<string> res;
    res.push_back(exIdentifier(c));
    c = c->sibling;
    while (c != NULL){
        res.push_back(exIdentifier(c));
        c = c->sibling;
    }
    return res;
}

Variable*& Execution::runFunction(VarFunction* var, vector<Variable *> arg){
    nowAR = var->ar;
    TreeNode* node = var->node;
    TreeNode* c = node->child;
    while (c != NULL){
        if (c->kind == FormalParameterList){
            vector<string>& vec = exFormalParameterList();
            for (int i=0; i<vec.size(); i++){
                if (i >= arg.size()) {
                    nowAR->setVar(vec[i], new VarUndefined());
                }
                else
                    nowAR->setVar(vec[i], arg[i]);
            }
            if (vec.size() < arg.size()){
                for (int i=vec.size(); i<arg.size(); i++)
                    arg[i]->release();
            }
        }
        if (c->kind == FunctionBody){
            Variable* result = exFunctionBody(c);         //incomplete
        }
        c = c->sibling;
    }
}

TreeNode*& Execution::exFunctionBody(TreeNode* t){
    this->testNode(t, FunctionBody, false);     //incomplete
    TreeNode* c = t->child;
    if (c == NULL) return new VarUndefined();
    else return exSourceElements(c);
}
