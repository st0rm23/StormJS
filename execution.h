#ifndef _EXECUTION_H_
#define _EXECUTION_H_
#include "global.h"
#include "variable.h"
#include <map>

using namespace std;

class ActiveRecord{
public:
    map<string, Variable*> mapVar;
    ActiveRecord* controlLink;
    ActiveRecord* accessLink;
    TreeNode* node;

    ActiveRecord(ActiveRecord* controlLink, ActiveRecord* accessLink, TreeNode* node);
    Variable*& getVar(const string &id);
    void setVar(string &id, Variable* var);
	static ActiveRecord* findAccessLink(TreeNode* t, ActiveRecord* nowAR);
};


class SymbolTable{
public:
	typedef map<string, TreeNode*> ST4Node;
	typedef map<TreeNode*, ST4Node*> MapST;
private:
    MapST mapST;

public:
    void insertSymbol(string &id, TreeNode* node);
    TreeNode* fetchSymbol(TreeNode* node, string &id);
};


class Execution{
private:
    ActiveRecord* globalAR;
    ActiveRecord* nowAR;
    SymbolTable* st;
	TreeNode* output;
public:
	Execution::Execution(TreeNode *node);
	void run();
	void Execution::testNode(TreeNode* t, NodeKind kind, bool needChild);
    string Execution::exIdentifier(TreeNode* t);
	TokenType Execution::exAssignmentOperator(TreeNode* t);
    void Execution::exProgram(TreeNode* t);
	Variable* Execution::exSourceElements(TreeNode* t);
	Variable* Execution::exSourceElement(TreeNode* t);
    void Execution::exFunctionDeclaration(TreeNode* t);
    Variable* Execution::exStatement(TreeNode* t);
    void Execution::exVariableStatement(TreeNode* t);
    void Execution::exVariableDeclarationList(TreeNode* t);
    void Execution::exVariableDeclaration(TreeNode* t);
    Variable* Execution::exInitialiser(TreeNode* t);
    Variable* Execution::exAssignmentExpression(TreeNode* t);
    Variable* Execution::exConditionalExpression(TreeNode* t);
    Variable* Execution::exLogicalORExpression(TreeNode* t);
	TokenType Execution::exLogicalOROperator(TreeNode* t);
    Variable* Execution::exLogicalANDExpression(TreeNode* t);
	TokenType Execution::exLogicalANDOperator(TreeNode* t);
    Variable* Execution::exBitwiseORExpression(TreeNode* t);
	TokenType Execution::exBitwiseOROperator(TreeNode* t);
    Variable* Execution::exBitwiseXORExpression(TreeNode* t);
	TokenType Execution::exBitwiseXOROperator(TreeNode* t);
    Variable* Execution::exBitwiseANDExpression(TreeNode* t);
	TokenType Execution::exBitwiseANDOperator(TreeNode* t);
    Variable* Execution::exEqualityExpression(TreeNode* t);
	TokenType Execution::exEqualityOperator(TreeNode* t);
    Variable* Execution::exRelationalExpression(TreeNode* t);
	TokenType Execution::exRelationalOperator(TreeNode* t);
    Variable* Execution::exShiftExpression(TreeNode* t);
	TokenType Execution::exShiftOperator(TreeNode* t);
    Variable* Execution::exAdditiveExpression(TreeNode* t);
	TokenType Execution::exAdditiveOperator(TreeNode* t);
    Variable* Execution::exMultiplicativeExpression(TreeNode* t);
	TokenType Execution::exMultiplicativeOperator(TreeNode* t);
    Variable*  Execution::exUnaryExpression(TreeNode* t);
	TokenType  Execution::exUnaryOperator(TreeNode* t);
    Variable* Execution::exPostfixExpression(TreeNode* t);
    TokenType Execution::exPostfixOperator(TreeNode* t);
    Variable* Execution::exLeftHandSideExpressionForValue(TreeNode* t);
	Variable*& Execution::exLeftHandSideExpressionForVar(TreeNode* t);
	vector<Variable*> Execution::exArguments(TreeNode* t);
	vector<Variable*> Execution::exArgumentList(TreeNode* t);
	Variable* Execution::exMemberExpressionForValue(TreeNode* t);
	Variable*& Execution::exMemberExpressionForVar(TreeNode* t);
	Variable* Execution::exPrimaryExpressionForValue(TreeNode* t);
	Variable*& Execution::exPrimaryExpressionForVar(TreeNode* t);
    Variable* Execution::exLiteral(TreeNode* t);
    vector<string> Execution::exFormalParameterList(TreeNode* t);
    Variable* Execution::exFunctionBody(TreeNode* t);
	void Execution::exExpressionStatement(TreeNode* t);
	Variable* Execution::exExpression(TreeNode* t);
	Variable* Execution::exReturnStatement(TreeNode* t);
	Variable* Execution::exIfStatement(TreeNode* t);
	Variable* Execution::runFunction(VarFunction* var, vector<Variable *> arg);
};

#endif
