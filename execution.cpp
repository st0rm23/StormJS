#include "execution.h"
#include "expression.h"


ActiveRecord::ActiveRecord(ActiveRecord* controlLink, ActiveRecord* accessLink, TreeNode* node){
    this->controlLink = controlLink;
    this->accessLink = accessLink;
    this->node = node;
}

Variable*& ActiveRecord::getVar(const string &id){
    ActiveRecord* ar = this;
    while (ar != NULL){
		map<string, Variable*>::iterator it = ar->mapVar.find(id);
		if (it != ar->mapVar.end()) {
			Variable*& var = it->second;
			var->retain();
			return var;
		}
        ar = ar->accessLink;
    }
	throw ExecutionException();
}

void ActiveRecord::setVar(string &id, Variable* var){
    Variable* old = NULL;
    if (mapVar.find(id) != mapVar.end()) old = mapVar[id];
    Variable* tmp = Variable::assign(&old, var);
    mapVar[id] = tmp;
    tmp->release();
}

ActiveRecord* ActiveRecord::findAccessLink(TreeNode* t, ActiveRecord* nowAR){
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
	return ar;
}


void SymbolTable::insertSymbol(string &id, TreeNode* node){
    TreeNode* p = node;
    p = p->parent;
    while (p != NULL){
        if (p->kind == FunctionDeclaration || p->kind == Program){
            ST4Node* st;
            if (mapST.find(p) == mapST.end()) st = mapST[p] = new ST4Node();
            else st = mapST[p];
            (*st)[id] = node;
            break;
        }
        p = p->parent;
    }
}

TreeNode* SymbolTable::fetchSymbol(TreeNode* node, string &id){
    TreeNode* p = node;
    p = p->parent;
    while (p != NULL){
        if (p->kind == FunctionDeclaration || p->kind == Program){
            if (mapST.find(p) != mapST.end()){
                ST4Node* st = mapST[p];
                if (st->find(id) != st->end()) return (*st)[id];
            }
        }
        p = p->parent;
    }
    return NULL;
}

Execution::Execution(TreeNode *node){
	this->nowAR = this->globalAR = new ActiveRecord(NULL, NULL, node);
	this->st = new SymbolTable();
	this->output = new TreeNode(-1, NodeKind::FunctionDeclaration, node);
	string str = "output";
	this->st->insertSymbol(str, output);
}

void Execution::testNode(TreeNode* t, NodeKind kind, bool needChild){
	if (t == NULL) throw ExecutionException();
    if (t->kind != kind) throw ExecutionException();
    if (needChild && t->child == NULL) throw ExecutionException();
}


string Execution::exIdentifier(TreeNode* t){
    this->testNode(t, Identifier, false);
    return t->token.value;
}

TokenType Execution::exAssignmentOperator(TreeNode* t){
    this->testNode(t, AssignmentOperator, false);
    return t->token.type;
}

void Execution::exProgram(TreeNode* t){
    this->testNode(t, Program, true);
    TreeNode* c = t->child;
    Variable* res = this->exSourceElements(c);
	res->release();
}

Variable* Execution::exSourceElements(TreeNode* t){
	Variable* res = NULL;
    this->testNode(t, SourceElements, true);
    TreeNode* c = t->child;
    res = this->exSourceElement(c);
	if (res == NULL){
		while ((c = c->sibling) != NULL){
			res = this->exSourceElement(c);
			if (res != NULL) break;
		}
	}
	if (res == NULL) return new VarUndefined();
	return res;
}

Variable* Execution::exSourceElement(TreeNode* t){
    this->testNode(t, SourceElement, true);
    TreeNode* c = t->child;
	if (c->kind == FunctionDeclaration)
	{
		this->exFunctionDeclaration(c);
		return NULL;
	}
	else {
		return this->exStatement(c);
	}
}

void Execution::exFunctionDeclaration(TreeNode* t){
    this->testNode(t, FunctionDeclaration, true);
    TreeNode* c = t->child;
    if (c->kind != Identifier) throw ExecutionException();
    string funcId = exIdentifier(c);
    this->st->insertSymbol(funcId, t);
}

Variable* Execution::exStatement(TreeNode* t){   //incomplete
    this->testNode(t, Statement, true);
    TreeNode* c = t->child;
    switch (c->kind){
        case VariableStatement:
            this->exVariableStatement(c);
            break;
		case ReturnStatement:
			return this->exReturnStatement(c);
			break;
		case IfStatement:
			return this->exIfStatement(c);
			break;
        default:
            exExpressionStatement(c);
            break;
    }
	return NULL;
}

Variable* Execution::exIfStatement(TreeNode* t){   //incomplete
	this->testNode(t, IfStatement, true);
	TreeNode* c = t->child;
	VarBoolean* bo = (VarBoolean*)exExpression(c);
	if (bo->toBoolean()) {
		return exStatement(c->sibling);
	}
	else{
		if (c->sibling->sibling != NULL)
			return exStatement(c->sibling->sibling);
		else return NULL;
	}
}

Variable* Execution::exReturnStatement(TreeNode* t){   //incomplete
	this->testNode(t, ReturnStatement, true);
	TreeNode* c = t->child;
	if (c == NULL) return new VarUndefined();
	else return exExpression(c);
}

void Execution::exVariableStatement(TreeNode* t){
    this->testNode(t, VariableStatement, true);
    TreeNode* c = t->child;
    this->exVariableDeclarationList(c);
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
	fprintf(stderr, "%s\n", id.c_str());
    Variable* var = NULL;
    c = c->sibling;
    if (c != NULL) var = this->exInitialiser(c);
    else var = new VarUndefined();

    this->nowAR->setVar(id, var);
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
		
		Variable* oldLeft;
        Variable*& left = exLeftHandSideExpressionForVar(leftNode);
		oldLeft = left;
        Variable* right = exAssignmentExpression(rightNode);
        TokenType op = exAssignmentOperator(opNode);

        Variable* res = Expression::opAssign(left, op, right);

		oldLeft->release();
        right->release();
        return res;
    }
    else {
        Variable* res = exConditionalExpression(c);
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
            TokenType op = exLogicalOROperator(c->sibling);
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

TokenType Execution::exLogicalOROperator(TreeNode* t){
    this->testNode(t, LogicalOROperator, false);
    return t->token.type;
}


Variable* Execution::exLogicalANDExpression(TreeNode* t){
    this->testNode(t, LogicalANDExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseORExpression(c);
        while (c->sibling != NULL){
            TokenType op = exLogicalANDOperator(c->sibling);
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

TokenType Execution::exLogicalANDOperator(TreeNode* t){
    this->testNode(t, LogicalANDOperator, false);
    return t->token.type;
}



Variable* Execution::exBitwiseORExpression(TreeNode* t){
    this->testNode(t, BitwiseORExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseXORExpression(c);
        while (c->sibling != NULL){
			TokenType op = exBitwiseOROperator(c->sibling);
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

TokenType Execution::exBitwiseOROperator(TreeNode* t){
    this->testNode(t, BitwiseOROperator, false);
    return t->token.type;
}

Variable* Execution::exBitwiseXORExpression(TreeNode* t){
    this->testNode(t, BitwiseXORExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exBitwiseANDExpression(c);
        while (c->sibling != NULL){
			TokenType op = exBitwiseXOROperator(c->sibling);
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

TokenType Execution::exBitwiseXOROperator(TreeNode* t){
    this->testNode(t, BitwiseXOROperator, false);
    return t->token.type;
}


Variable* Execution::exBitwiseANDExpression(TreeNode* t){
    this->testNode(t, BitwiseANDExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exEqualityExpression(c);
        while (c->sibling != NULL){
			TokenType op = exBitwiseANDOperator(c->sibling);
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

TokenType Execution::exBitwiseANDOperator(TreeNode* t){
    this->testNode(t, BitwiseANDOperator, false);
    return t->token.type;
}

Variable* Execution::exEqualityExpression(TreeNode* t){
	this->testNode(t, EqualityExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exRelationalExpression(c);
        while (c->sibling != NULL){
			TokenType op = exEqualityOperator(c->sibling);
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

TokenType Execution::exEqualityOperator(TreeNode* t){
    this->testNode(t, EqualityOperator, false);
    return t->token.type;
}

Variable* Execution::exRelationalExpression(TreeNode* t){
    this->testNode(t, RelationalExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exShiftExpression(c);
        while (c->sibling != NULL){
			TokenType op = exRelationalOperator(c->sibling);
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

TokenType Execution::exRelationalOperator(TreeNode* t){
    this->testNode(t, RelationalOperator, false);
    return t->token.type;
}

Variable* Execution::exShiftExpression(TreeNode* t){
    this->testNode(t, ShiftExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exAdditiveExpression(c);
        while (c->sibling != NULL){
			TokenType op = exShiftOperator(c->sibling);
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

TokenType Execution::exShiftOperator(TreeNode* t){
    this->testNode(t, ShiftOperator, false);
    return t->token.type;
}

Variable* Execution::exAdditiveExpression(TreeNode* t){
    this->testNode(t, AdditiveExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exMultiplicativeExpression(c);
        while (c->sibling != NULL){
            TokenType op = exAdditiveOperator(c->sibling);
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

TokenType Execution::exAdditiveOperator(TreeNode* t){
    this->testNode(t, AdditiveOperator, false);
    return t->token.type;
}

Variable* Execution::exMultiplicativeExpression(TreeNode* t){
    this->testNode(t, MultiplicativeExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
        Variable *a = exUnaryExpression(c);
        while (c->sibling != NULL){
			TokenType op = exMultiplicativeOperator(c->sibling);
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


TokenType Execution::exMultiplicativeOperator(TreeNode* t){
    this->testNode(t, MultiplicativeOperator, false);
    return t->token.type;
}


Variable*  Execution::exUnaryExpression(TreeNode* t){
    this->testNode(t, UnaryExpression, true);
    TreeNode* c = t->child;
    if (c->sibling != NULL){
		TokenType op = exUnaryOperator(c);
        Variable* tmp = exUnaryExpression(c->sibling);
        Variable* res = Expression::opUnary(op, tmp);
        tmp->release();
        return res;
    } else{
        return exPostfixExpression(c);
    }
}


TokenType  Execution::exUnaryOperator(TreeNode* t){
    this->testNode(t, UnaryOperator, false);
    return t->token.type;
}

Variable* Execution::exPostfixExpression(TreeNode* t){
    this->testNode(t, PostfixExpression, true);
    TreeNode* c = t->child;

    if (c->sibling != NULL){
		Variable*& res = exLeftHandSideExpressionForVar(c);
		TokenType op = exPostfixOperator(c);
        Variable* a = Expression::opPostfix(res, op);
        res->release();
        return a;
    }
    else return exLeftHandSideExpressionForValue(c);
}

TokenType Execution::exPostfixOperator(TreeNode* t){
    this->testNode(t, PostfixOperator, false);
    return t->token.type;
}

Variable* Execution::exLeftHandSideExpressionForValue(TreeNode* t){      //LeftHandSideExpression -> MemberExpression [ Arguments ( CallExpressionPart )* ];
	this->testNode(t, LeftHandSideExpression, true);

	TreeNode* c = t->child;

	if (c->sibling != NULL){        //it's a calling
		Variable* res = exMemberExpressionForValue(c);
		if (res->type != Variable::FUNCTION) throw ExecutionException();

		vector<Variable*>& vec = exArguments(c->sibling);
		VarFunction *vf = (VarFunction*)res;
		if (vf->ar == NULL) {
			vf->ar = new ActiveRecord(nowAR, ActiveRecord::findAccessLink(t, nowAR), vf->treeNode);
		}
		Variable* var = runFunction(vf, vec);			//incomplete
		res->release();
		return var;
	}
	else if (c->child->child->kind == Expression || c->child->child->kind == Literal
		|| c->child->child->kind == Identifier){
		return exMemberExpressionForValue(c);								//problem
	}
	else throw ExecutionException();
}


Variable*& Execution::exLeftHandSideExpressionForVar(TreeNode* t){      //LeftHandSideExpression -> MemberExpression [ Arguments ( CallExpressionPart )* ];
    this->testNode(t, LeftHandSideExpression, true);
    TreeNode* c = t->child;

	if (c->sibling != NULL){        //it's a calling
		throw ExecutionException();
	}
	else if (c->child->child->kind == Expression || c->child->child->kind == Literal){
		throw ExecutionException();							//problem
	}
	else return exMemberExpressionForVar(c);
}

vector<Variable*> Execution::exArguments(TreeNode* t){
    this->testNode(t, Arguments, false);
    TreeNode* c = t->child;
    if (c == NULL) {
        vector<Variable*> res;
        return res;
    } else {
        return exArgumentList(c);
    }
}

vector<Variable*> Execution::exArgumentList(TreeNode* t){
	this->testNode(t, ArgumentList, true);
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


Variable* Execution::exMemberExpressionForValue(TreeNode* t){
    this->testNode(t, MemberExpression, true);
    TreeNode* c = t->child;
	Variable* res = exPrimaryExpressionForValue(c);         //incomplete
    return res;
}

Variable*& Execution::exMemberExpressionForVar(TreeNode* t){
	this->testNode(t, MemberExpression, true);
	TreeNode* c = t->child;
	Variable*& res = exPrimaryExpressionForVar(c);         //incomplete
	return res;
}

Variable* Execution::exPrimaryExpressionForValue(TreeNode* t){
    this->testNode(t, PrimaryExpression, true);     //incomplete
    TreeNode* c = t->child;
	if (c->kind == Identifier){
		string id = exIdentifier(c);
		if (this->st->fetchSymbol(t, id) != NULL){	//it's a function name
			TreeNode *node = this->st->fetchSymbol(t, id);
			VarFunction* var = new VarFunction();
			var->value = node;
			var->ar = new ActiveRecord(nowAR, ActiveRecord::findAccessLink(t, nowAR), node);
			return var;
		}
		else {
			Variable* var = this->nowAR->getVar(id);
			return var;
		}
	}
	else if (c->kind == Literal){
		return exLiteral(c);
	}
	else return exExpression(c);
}

Variable*& Execution::exPrimaryExpressionForVar(TreeNode* t){
	this->testNode(t, PrimaryExpression, true);     //incomplete
	TreeNode* c = t->child;

	if (c->kind == Identifier){
		string id = exIdentifier(c);
		return this->nowAR->getVar(id);
	}
	else throw ExecutionException();
}

Variable* Execution::exLiteral(TreeNode* t){
    this->testNode(t, Literal, false);
    Variable * res = Variable::variableFromTreeNode(t);
    return res;
}


vector<string> Execution::exFormalParameterList(TreeNode* t){
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

Variable* Execution::runFunction(VarFunction* var, vector<Variable *> arg){
    nowAR = var->ar;
	Variable* res = NULL;
	if (var->value == output){ //output
		for (int i = 0; i < arg.size(); i++){
			Variable* p = arg[i];
			printf("%s%c", p->toString().c_str(), i == arg.size()-1 ? '\n':',');
		}
	}
	else {
		TreeNode* node = var->value;
		TreeNode* c = node->child;
		vector<string> vec;
		while (c != NULL){
			if (c->kind == FormalParameterList){
				vec = exFormalParameterList(c);
				for (int i = 0; i<vec.size(); i++){
					if (i >= arg.size()) {
						nowAR->setVar(vec[i], new VarUndefined());
					}
					else
						nowAR->setVar(vec[i], arg[i]);
				}
			}
			c = c->sibling;
		}

		c = node->child;
		while (c != NULL){
			if (c->kind == FunctionBody){
				res = exFunctionBody(c);         //incomplete
				break;
			}
			c = c->sibling;
		}
	}
	nowAR = nowAR->controlLink;
	for (int i = 0; i < arg.size(); i++) arg[i]->release();
	if (res == NULL) res = new VarUndefined();
	return res;
}

Variable* Execution::exFunctionBody(TreeNode* t){
    this->testNode(t, FunctionBody, false);     //incomplete
    TreeNode* c = t->child;
    if (c == NULL) return new VarUndefined();
    else return exSourceElements(c);
}


void Execution::exExpressionStatement(TreeNode* t){
	this->testNode(t, ExpressionStatement, true);    
	TreeNode* c = t->child;
	while (c != NULL){
		Variable* var = exExpression(c);
		var->release();
		c = c->sibling;
	}
}

Variable* Execution::exExpression(TreeNode* t){
	this->testNode(t, Expression, true);     //incomplete
	TreeNode* c = t->child;
	while (c != NULL){
		if (c->sibling == NULL) return exAssignmentExpression(c);
		Variable* tmp = exAssignmentExpression(c);
		tmp->release();
		c = c->sibling;
	}
}







/*




TreeNode* Execution::exMemberExpressionPart(TreeNode* t){
	TreeNode* t = new TreeNode(scanner->getLine(), MemberExpressionPart);
    TreeNode* c = NULL;

    if (token.type == LPMID){
        match(LPMID);
        c = t->child = syExpression();
        match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier();
    }
    return t;
}

TreeNode* Execution::exCallExpressionPart(TreeNode* t){
    TreeNode* t = new TreeNode(scanner->getLine(), CallExpressionPart);
    TreeNode* c = NULL;
    if (token.type == LPSMALL)
        c = t->child = syArguments();
    else if (token.type == LPMID) {
            match(LPMID);
            c = t->child = syExpression();
            match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier();
    }
    return t;
}

TreeNode* Execution::exFunctionExpression(TreeNode* t){
    TreeNode* t = new TreeNode(scanner->getLine(), FunctionExpression);
    TreeNode* c = NULL;
    match(FUNCTION);
    if (token.type == ID) c = t->child = syIdentifier();
    else c = t->child = new TreeNode(scanner->getLine(), Identifier);
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = t->child = syFormalParameterList();
    match(RPSMALL);
    c = c->sibling = syFunctionBody();
    return t;
}

TreeNode* Execution::exArrayLiteral(TreeNode* t){
    TreeNode* t = new TreeNode(scanner->getLine(), ArrayLiteral);
    TreeNode* c = NULL;
    match(LPMID);
    while (token.type != RPMID){
        if (token.type == COMMA)
            if (t->child == NULL) c = t->child = syElision();
            else c = c->sibling = syElision();
        else {
            if (t->child == NULL) c = t->child = syAssignmentExpression();
            else c = c->sibling = syAssignmentExpression();
            if (token.type != ']') match(COMMA);
        }
    }
    match(RPMID);
    return t;
}

TreeNode* Execution::exElision(TreeNode* t){
    TreeNode* t = new TreeNode(scanner->getLine(), Elision);
    TreeNode* c = NULL;
    t->token.value = "";
	t->token.type = COMMA;
	match(COMMA);
	t->token.value += ',';
    while (token.type == COMMA){
		t->token.value += ',';
        match(COMMA);
    }
    return t;
}
*/
