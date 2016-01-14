#ifndef _EXPRESSION_H_
#define _EXPRESSION_H_

#include "global.h"
#include "variable.h"

class Expression{
public:
    static Variable* opAssign(Variable *&a, TokenType op, Variable *b);
    static Variable* opCondition(Variable *a, Variable *b, Variable *c);
    static Variable* opLogical(Variable *a, TokenType op, Variable *b);
    static Variable* opBitwise(const Variable *a, TokenType op, const Variable *b);
    static Variable* opEqual(Variable *a, TokenType op, Variable*b);
    static Variable* opRelation(Variable *a, TokenType op, Variable *b);
    static Variable* opShift(const Variable *a, TokenType op, const Variable *b);
    static Variable* opAdditive(Variable *a, TokenType op, Variable *b);
    static Variable* opMultiplicative(Variable *a, TokenType op, Variable *b);
    static Variable* opUnary(TokenType op, Variable *a);
    static Variable* opPostfix(Variable *a, TokenType op);
};

#endif
