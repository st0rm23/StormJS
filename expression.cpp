#include "expression.h"

Variable* Expression::opAssign(Variable *&a, TokenType op, Variable *b){
    Variable* res = NULL;
    switch (op){
        case ASSIGN :{                  //    =
            res = Variable::assign(&a, b);
            break;
        }
		case PASSIGN: {                //    +=
			Variable* tmp = opAdditive(a, TokenType::PLUS, b);
			res = Variable::assign(&a, tmp);
			tmp->release();
			break;
		}
        case MINUSASSIGN :{            //    -=
			Variable* tmp = opAdditive(a, TokenType::MINUS, b);
			res = Variable::assign(&a, tmp);
            tmp->release();
            break;
        }
		case TASSIGN:{              //    *=
			Variable* tmp = opMultiplicative(a, TokenType::TIMES, b);
			res = Variable::assign(&a, tmp);
			tmp->release();
			break;
		}                
		case OASSIGN: {              //    /=
			Variable* tmp = opMultiplicative(a, TokenType::OVER, b);
			res = Variable::assign(&a, tmp);
			tmp->release();
			break;
		}                
        case MODASSIGN :{              //    %=
			Variable* tmp = opMultiplicative(a, TokenType::MOD, b);
			res = Variable::assign(&a, tmp);
            tmp->release();
            break;
        }
        case ORASSIGN : {              //    |=
			Variable* tmp = opMultiplicative(a, TokenType::OR, b);
			res = Variable::assign(&a, tmp);
            tmp->release();
            break;
        }
        case XORASSIGN :{              //    ^=
			Variable* tmp = opMultiplicative(a, TokenType::XOR, b);
			res = Variable::assign(&a, tmp);
            tmp->release();
            break;
        }
        case ANDASSIGN :{              //    &=
			Variable * tmp = opLogical(a, TokenType::AND, b);
			res = Variable::assign(&a, tmp);
            tmp->release();
            break;
        }
        default :{
            throw ExpressionException();
            break;
        }
    }
    return res;
}

Variable* Expression::opCondition(Variable *a, Variable *b, Variable *c){
    bool bo = a->toBoolean();
    if (bo) return b;
    else return c;
}

Variable* Expression::opLogical(Variable *a, TokenType op, Variable *b){
    VarBoolean* res = new VarBoolean();
    VarBoolean* ta = a->toVarBoolean();
    VarBoolean* tb = b->toVarBoolean();
    switch (op){
        case OROR :                    //    ||
            res->value = ta || tb;
        break;
        case ANDAND :                  //    &&
            res->value = ta && tb;
        break;
        default :
            break;
    }
    ta->release();
    tb->release();
    return res;
}

Variable* Expression::opBitwise(const Variable *a, TokenType op, const Variable *b){
	Variable* res = NULL;
    switch (op){
        case OR :                      //    |
            break;
        case AND :                     //    &
            break;
        case XOR :                     //    ^
            break;
        default :
            break;
    }
	return res;
}

Variable* Expression::opEqual(Variable *a, TokenType op, Variable*b){
	if (op != TokenType::EQUAL && op != TokenType::UNEQUAL)
        throw ExpressionException();

	VarBoolean* res = new VarBoolean();
    bool isEqual = false;
    if (a->type == Variable::BOOLEAN || a->type == Variable::NUMBER  //convert to number for comparison
     || b->type == Variable::BOOLEAN || b->type == Variable::NUMBER){
         VarNumber* ta = a->toVarNumber();
		 VarNumber* tb = b->toVarNumber();

         if (ta->numType == VarNumber::NANUMBER || tb->numType == NANUMBER)
            isEqual = (op != TokenType::EQUAL);
         else isEqual = (ta->toString() == tb->toString());

         ta->release();
         tb->release();
     } else if (a->type == Variable::OBJECT && ((VarObject*)a)->value != NULL
		 && b->type == Variable::OBJECT && ((VarObject*)b)->value != NULL)
         isEqual = (a == b);
      else isEqual = a->toString() == b->toString();

    switch (op){
        case EQUAL :{                   //    ==
            res->value = isEqual;
            break;
        }
        case UNEQUAL :{                 //    !=
            res->value = !isEqual;
            break;
        }
        default:
            throw ExpressionException();
            break;
    }
    return res;
}

Variable* Expression::opRelation(Variable *a, TokenType op, Variable *b){
	VarBoolean* res = new VarBoolean();

    if (a->type == Variable::BOOLEAN || a->type == Variable::NUMBER  //convert to number for comparison
     || b->type == Variable::BOOLEAN || b->type == Variable::NUMBER){
         VarNumber* ta = a->toVarNumber();
		 VarNumber* tb = b->toVarNumber();

         if (ta->numType == VarNumber::NANUMBER || tb->numType == VarNumber::NANUMBER)
            res->value = false;
         else {
             switch (op){
                 case LESS :                    //    <
                    res->value = (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::INF)
                              || (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::NORMAL)
                              || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::INF)
                              || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NORMAL
                              &&  ta->toNumber() < tb->toNumber());
                    break;
                 case GREAT :                   //    >
                    res->value = (ta->type == VarNumber::INF && tb->type == VarNumber::NEG_INF)
                           || (ta->type == VarNumber::INF && tb->type == VarNumber::NORMAL)
                           || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NEG_INF)
                           || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NORMAL
                           &&  ta->toNumber() > tb->toNumber());
                     break;
                 case LESSEUQAL :               //    <=
                    res->value = (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::INF)
                           || (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::NORMAL)
                           || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::INF)
                           || (ta->type == VarNumber::INF && tb->type == VarNumber::INF)
                           || (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::NEG_INF)
                           || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NORMAL
                           &&  ta->toNumber() <= tb->toNumber());
                     break;
                 case GREATQUAL :               //    >=
                 res->value = (ta->type == VarNumber::INF && tb->type == VarNumber::NEG_INF)
                        || (ta->type == VarNumber::INF && tb->type == VarNumber::NORMAL)
                        || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NEG_INF)
                        || (ta->type == VarNumber::INF && tb->type == VarNumber::INF)
                        || (ta->type == VarNumber::NEG_INF && tb->type == VarNumber::NEG_INF)
                        || (ta->type == VarNumber::NORMAL && tb->type == VarNumber::NORMAL
                        &&  ta->toNumber() >= tb->toNumber());
                     break;
                 default :
                     throw ExpressionException();
                     break;
             }
         }
         ta->release();
         tb->release();
     } else {
         switch (op){
             case LESS :                    //    <
                 res->value = a->toString() < b->toString();
                 break;
             case GREAT :                   //    >
                 res->value = a->toString() > b->toString();
                 break;
             case LESSEUQAL :               //    <=
                 res->value = a->toString() <= b->toString();
                 break;
             case GREATQUAL :               //    >=
                 res->value = a->toString() >= b->toString();
                 break;
             default :
                 throw ExpressionException();
                 break;
         }
     }
     return res;
}

Variable* Expression::opShift(const Variable *a, TokenType op, const Variable *b){
	Variable* res = NULL;
	switch (op){
        case LSHIFT :                  //    <<

            break;
        case RSHIFT :                  //    >>
            break;
        default :
            break;
    }
	return res;
}

Variable* Expression::opAdditive(Variable *a, TokenType op, Variable *b){
    Variable *res = NULL;
    if ((a->type == Variable::STRING || a->type == Variable::FUNCTION ||
        b->type == Variable::STRING || b->type == Variable::FUNCTION)
        && op == TokenType::PLUS){
		VarString* tmp = new VarString();
		tmp->value = a->toString() + b->toString();
		res = tmp;
    } else{
		VarNumber* tmp = new VarNumber();
		VarNumber* ta = a->toVarNumber();
		VarNumber* tb = b->toVarNumber();
		if (ta->numType == VarNumber::NANUMBER || tb->numType == VarNumber::NANUMBER){
			tmp->numType = VarNumber::NANUMBER;
			res = tmp;
		}
        else {
            switch (op){
                case MINUS :                  // -
                    if (tb->numType == VarNumber::INF) tb->numType = VarNumber::NEG_INF;
                    else if (tb->numType == VarNumber::NEG_INF) tb->numType = VarNumber::INF;
                    else tb->value = -tb->value;
                case PLUS :                    //    +
                    if (ta->numType == VarNumber::INF && tb->numType == VarNumber::NEG_INF)
						tmp->numType = VarNumber::NANUMBER;
                    else if (ta->numType == VarNumber::NEG_INF && tb->numType == VarNumber::INF)
						tmp->numType = VarNumber::NANUMBER;
                    else if (ta->numType == VarNumber::NEG_INF || tb->numType == VarNumber::NEG_INF)
						tmp->numType = VarNumber::NEG_INF;
                    else if (ta->numType == VarNumber::INF || tb->numType == VarNumber::INF)
						tmp->numType = VarNumber::INF;
                    else {
						tmp->numType = VarNumber::NORMAL;
						tmp->value = ta->value + tb->value;
                    }
					res = tmp;
                    break;
                default :
                    throw ExpressionException();
                    break;
            }
        }
        ta->release();
        tb->release();
    }
    return res;
}

Variable* Expression::opMultiplicative(Variable *a, TokenType op, Variable *b){

    VarNumber* res = new VarNumber();
	VarNumber* ta = a->toVarNumber();
	VarNumber* tb = b->toVarNumber();
    if (ta->numType == VarNumber::NANUMBER || tb->numType == VarNumber::NANUMBER)
       res->numType = VarNumber::NANUMBER;
    else {
        bool nega = ta->numType == VarNumber::NEG_INF
                || (ta->numType == VarNumber::NORMAL && ta->value < 0);
        bool negb = tb->numType == VarNumber::NEG_INF
                || (tb->numType == VarNumber::NORMAL && tb->value < 0);
        bool negres = nega != negb;

        switch (op){
            case TIMES :                   //    *
                if (ta->numType == VarNumber::INF && tb->numType == VarNumber::NORMAL && fabs(tb->value) <1e-9)
                    res->numType = VarNumber::NANUMBER;
                else if (tb->numType == VarNumber::INF && ta->numType == VarNumber::NORMAL && ta->value <1e-9)
                    res->numType = VarNumber::NANUMBER;
                else {

                    if (ta->numType != VarNumber::NORMAL)
                        res->numType = negres ? VarNumber::NEG_INF : VarNumber::INF;
                    else if (tb->numType != VarNumber::NORMAL)
                        res->numType = negres ? VarNumber::NEG_INF : VarNumber::INF;
                    else {
						res->numType = VarNumber::NORMAL;
                        res->value = ta->value * tb->value;
                    }
                }
                break;
            case OVER :                    //    /
                if (ta->numType != VarNumber::NORMAL && tb->numType != VarNumber::NORMAL)
                    res->numType = VarNumber::NANUMBER;
                else if (ta->numType != VarNumber::NORMAL)
                    res->numType = negres ? VarNumber::NEG_INF : VarNumber::INF;
                else if (tb->numType != VarNumber::NORMAL){
                    res->numType = VarNumber::NORMAL;
                    res->value = 0;
                } else if (fabs(tb->value) < 1e-9)
					res->numType = VarNumber::NANUMBER;
                else {
                    res->numType = VarNumber::NORMAL;
                    res->value = ta->value / tb->value;
                }
                break;
            case MOD :                     //    %
                if (ta->numType != VarNumber::NORMAL && tb->numType != VarNumber::NORMAL)
                    res->numType = VarNumber::NANUMBER;
                else if (ta->numType != VarNumber::NORMAL
                        || (tb->numType == VarNumber::NORMAL && fabs(tb->value) < 1e-9
                            || tb->numType == VarNumber::INF))
                    res->numType = VarNumber::NANUMBER;
                else if (tb->numType != VarNumber::NORMAL){
                    res->numType = VarNumber::NORMAL;
                    res->value = ta->value;
                } else if (ta->value == 0){
                    res->numType = VarNumber::NORMAL;
                    res->value = 0;
				}
                else {
                    res->numType = VarNumber::NORMAL;
					res->value = (long long)(ta->value) % (long long)(tb->value);
                }
                break;
            default :
                throw ExpressionException();
                break;
        }
    }
    ta->release();
    tb->release();
    return res;
}

Variable* Expression::opUnary(TokenType op, Variable *a){
    Variable* res = NULL;

    switch (op){
        case NOT :{                     //    !
            VarBoolean* tmp = new VarBoolean();
            tmp->value = !a->toBoolean();
			res = tmp;
			break;
        }
        case INCREASE : {               //    ++
            VarNumber* tmp = new VarNumber();
            tmp->value = 1;
            res = opAdditive(a, TokenType::PLUS, tmp);
            tmp->release();
            break;
        }
        case DECREASE : {               //    --
            VarNumber* tmp = new VarNumber();
            tmp->value = 1;
            res = opAdditive(a, TokenType::MINUS, tmp);
            tmp->release();
            break;
        }
        case REVERSE :{                 //    ~

            break;
        }
        case PLUS :  {                  //    +
            VarNumber* tmp = a->toVarNumber();
			res = tmp;
            break;
        }
        case MINUS :  {                 //    -
            VarNumber* tmp = new VarNumber();
            res = opAdditive(tmp, TokenType::MINUS, a);
            tmp->release();
            break;
        }
        default :
            throw ExpressionException();
            break;
    }
    return res;
}

Variable* Expression::opPostfix(Variable *&a, TokenType op){
    VarNumber* tmp = new VarNumber();
    tmp->value = 1;
    Variable* res = a;
    res->retain();
    switch (op){
        case INCREASE : {               //    ++
            Variable* tt = opAdditive(a, TokenType::PLUS, tmp);
			Variable* tt2 = Variable::assign(&a, tt);
            tt->release();
            tt2->release();
            break;
        }
        case DECREASE :{                //    --
            Variable* tt = opAdditive(a, TokenType::MINUS, tmp);
			Variable* tt2 = Variable::assign(&a, tt);
            tt->release();
            tt2->release();
            break;
        }
        default :
            throw ExpressionException();
            break;
    }
    tmp->release();
    return res;
}
