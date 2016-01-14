#include "variable.h"


Variable::Variable(){
    this->_retain = 1;
    this->type = UNDEFINED;
    this->treeNode = NULL;
}

void Variable::retain(){
    _retain++;
}

void Variable::release(){
    _retain--;
    if (_retain == 0) delete this;
}


Variable* Variable::variableFromToken(const Token *token){
    Variable* res;
    switch (token->type) {
        case TokenType::INTEGER:
        case TokenType::DECIMAL:
        case TokenType::HEX:{
                VarNumber* num = new VarNumber();
                num->value = Util::strtoNum(token->value);
                res = num;
            }
            break;
        case TokenType::STRING:{
                VarString* str = new VarString();
                str->value = token->value;
                res = str;
            }
            break;
        case TokenType::VBOOLEAN:{
                VarBoolean* bo = new VarBoolean();
                bo->value = token->value == "true";
                res = bo;
            }
            break;
        case TokenType::INF:{
                VarNumber* num = new VarNumber();
                num->numType = VarNumber::INF;
                res = num;
            }
			break;
		case TokenType::NANUMBER:{
                VarNumber* num = new VarNumber();
                num->numType = VarNumber::NANUMBER;
                res = num;
            }
			break;
		case TokenType::NIL:{
				VarObject* obj = new VarObject();
				res = obj;
            }
			break;
		case TokenType::UNDEFINED:{
                VarUndefined* undefined = new VarUndefined();
                res = undefined;
            }
			break;
        default:
            throw ExpressionException();
            break;
    }
    return res;
}

Variable* Variable::variableFromTreeNode(const TreeNode *node){
    Variable *res;
    const Token *token = &(node->token);
    switch(node->kind){
        case NodeKind::Literal:
            res = variableFromToken(token);
            break;
        case NodeKind::FunctionBody:{
                VarFunction* func = new VarFunction();
                res = func;
            }
            break;
        default:
            throw ExpressionException();
    }
    return res;
}

string Variable::toString(){
	VarString* tmp = this->toVarString();
	string value = tmp->value;
	tmp->release();
	return value;
}

double Variable::toNumber(){
	VarNumber* tmp = this->toVarNumber();
	double value = tmp->value;
	tmp->release();
	return value;
}

bool Variable::toBoolean(){
	VarBoolean* tmp = this->toVarBoolean();
	bool value = tmp->value;
	tmp->release();
	return value;
}

VarString::VarString(){
    this->type = Variable::STRING;
    this->value = "";
}

VarBoolean* VarString::toVarBoolean(){
    VarBoolean* res = new VarBoolean();
    if (this->value == "") res->value = false;
    else res->value = true;
    return res;
}

VarString* VarString::toVarString(){
    this->retain();
    return this;
}

VarNumber* VarString::toVarNumber(){
    VarNumber* res = new VarNumber();
    if (this->value == "-Infinity")
        res->numType = VarNumber::NEG_INF;
    else if (this->value == "Infinity")
        res->numType = VarNumber::INF;
    else {
        try{
            res->value = Util::strtoNum(this->value);
        } catch (ExpressionException e){
            res->numType = VarNumber::NANUMBER;
        }
    }
    return res;
}



VarNumber::VarNumber(){
    this->type = Variable::NUMBER;
    this->numType = NORMAL;
    this->value = 0;
}

VarBoolean* VarNumber::toVarBoolean(){
    VarBoolean* res = new VarBoolean();
    switch (numType) {
        case NORMAL:
            if (fabs(this->value) < 1e-9) res->value = false;
            else res->value = true;
            break;
        case NANUMBER:
            res->value = false;
            break;
        case INF:
        case NEG_INF:
            res->value = true;
            break;
        default:
            throw ExpressionException();
    }
    return res;
}

VarString* VarNumber::toVarString(){
    VarString* res = new VarString();
    switch (numType) {
        case NORMAL:
            res->value = Util::numtoStr(this->value);
            break;
        case NANUMBER:
            res->value = "NaN";
            break;
        case INF:
            res->value = "Infinity";
            break;
        case NEG_INF:
            res->value = "-Infinity";
            break;
        default:
            throw ExpressionException();
    }
    return res;
}

VarNumber* VarNumber::toVarNumber(){
    this->retain();
    return this;
}




VarBoolean::VarBoolean(){
    this->type = Variable::BOOLEAN;
    this->value = false;
}

VarBoolean* VarBoolean::toVarBoolean(){
    this->retain();
    return this;
}

VarString* VarBoolean::toVarString(){
    VarString* res = new VarString();
    if (this->value == true) res->value = "true";
    else res->value = "false";
    return res;
}

VarNumber* VarBoolean::toVarNumber(){
    VarNumber* res = new VarNumber();
    if (this->value == true) res->value = 1;
    else res->value = 0;
    return res;
}



VarUndefined::VarUndefined(){
    this->type = Variable::UNDEFINED;
}

VarBoolean* VarUndefined::toVarBoolean(){
    VarBoolean* res = new VarBoolean();
    res->value = false;
    return res;
}

VarString* VarUndefined::toVarString(){
    VarString* res = new VarString();
    res->value = "undefined";
    return res;
}

VarNumber* VarUndefined::toVarNumber(){
    VarNumber* res = new VarNumber();
    res->numType = VarNumber::NANUMBER;
    return res;
}

VarObject::VarObject(){
    this->type = Variable::OBJECT;
    this->value = NULL;
}

VarBoolean* VarObject::toVarBoolean(){
    VarBoolean* res = new VarBoolean();
    res->value = true;
    return res;
}

VarString*  VarObject::toVarString(){
    VarString* res = new VarString();
    if (this->value == NULL)
        res->value = "null";
    else
        res->value = "object";
    return res;
}

VarNumber*  VarObject::toVarNumber(){
    VarNumber* res = new VarNumber();
    res->numType = VarNumber::NANUMBER;
    return res;
}


VarFunction::VarFunction(){
    this->type = Variable::FUNCTION;
}

VarBoolean* VarFunction::toVarBoolean(){
    VarBoolean* res = new VarBoolean();
    res->value = true;
    return res;
}

VarString* VarFunction::toVarString(){
    VarString* res = new VarString();
    res->value = "function";
    return res;
}

VarNumber* VarFunction::toVarNumber(){
    VarNumber* res = new VarNumber();
    res->numType = VarNumber::NANUMBER;
    return res;
}

Variable* Variable::assign(Variable** a, Variable* b){
    Variable *res = b;
    res->retain();
    if (*a == b) return res;
    if (*a != NULL) (*a)->release();	          //decrease refrence number to old var
    (*a) = b;							      //change pointer to new var
    (*a)->retain();					          //increase new var's reference number
    return res;						          //return new var's refrence
}
