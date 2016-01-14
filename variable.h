#ifndef _VARIABLE_H_
#define _VARIABLE_H_

#include "global.h"
#include "util.h"
#include <cmath>

class Variable;
class VarString;
class VarNumber;
class VarBoolean;
class VarUndefined;
class VarObject;
class VarFunction;

class Variable{
public:
    typedef enum{
        STRING, NUMBER, BOOLEAN, OBJECT, FUNCTION, UNDEFINED
    } VarType;

    TreeNode* treeNode;
    VarType type;
    int _retain;
    Variable();
    void retain();
    void release();

    virtual VarBoolean* toVarBoolean() = 0;
    virtual VarString* toVarString() = 0;
    virtual VarNumber* toVarNumber() = 0;
	string toString();
	double toNumber();
	bool toBoolean();

    static Variable* variableFromToken(const Token *token);
    static Variable* variableFromTreeNode(const TreeNode *node);
    static Variable* assign(Variable** a, Variable* b);
};

class VarString : public Variable{
public:
    string value;

    VarString();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};

class VarNumber : public Variable{
public:
    typedef enum{
        NORMAL, INF, NEG_INF, NANUMBER
    } VarNumberType;

    VarNumberType numType;
    double value;

    VarNumber();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};


class VarBoolean : public Variable{
public:
    bool value;
    VarBoolean();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};

class VarUndefined : public Variable{
public:
    VarUndefined();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};

class VarObject : public Variable{
public:
    void *value;

    VarObject();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};

class VarFunction : public Variable{
public:
    VarFunction();
    VarBoolean* toVarBoolean();
    VarString* toVarString();
    VarNumber* toVarNumber();
};

#endif
