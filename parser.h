#ifndef _PARSER_H_
#define _PARSER_H_

#include "scanner.h"
#include "global.h"

class Parser{
private:
    Token token;
    Scanner *scanner;
	vector<TreeNode*> reservedLeftExpressions;

public:
    Parser(FILE* stream);
    ~Parser();
    void match(const TokenType tokenType);
	void Parser::printError();

    TreeNode* syFunctionDeclaration(TreeNode* t);
    TreeNode* syIdentifier(TreeNode* t);
    TreeNode* syFormalParameterList(TreeNode* t);
    TreeNode* syFunctionBody(TreeNode* t);
    TreeNode* sySourceElements(TreeNode* t);
    TreeNode* sySourceElement(TreeNode* t);
    TreeNode* syStatement(TreeNode* t);
    TreeNode* syExpressionStatement(TreeNode* t);
    TreeNode* syExpression(TreeNode* t);
    TreeNode* syAssignmentExpression(TreeNode* t);
    TreeNode* syLeftHandSideExpression(TreeNode* t);
    TreeNode* syMemberExpression(TreeNode* t);
    TreeNode* syMemberExpressionPart(TreeNode* t);
    TreeNode* syCallExpressionPart(TreeNode* t);
    TreeNode* syArguments(TreeNode* t);
    TreeNode* syArgumentList(TreeNode* t);
    TreeNode* syFunctionExpression(TreeNode* t);
    TreeNode* syAssignmentOperator(TreeNode* t);
    TreeNode* syConditionalExpression(TreeNode* t);
    TreeNode* syLogicalORExpression(TreeNode* t);
    TreeNode* syLogicalOROperator(TreeNode* t);
    TreeNode* syLogicalANDExpression(TreeNode* t);
    TreeNode* syLogicalANDOperator(TreeNode* t);
    TreeNode* syBitwiseORExpression(TreeNode* t);
    TreeNode* syBitwiseOROperator(TreeNode* t);
    TreeNode* syBitwiseXORExpression(TreeNode* t);
    TreeNode* syBitwiseXOROperator(TreeNode* t);
    TreeNode* syBitwiseANDExpression(TreeNode* t);
    TreeNode* syBitwiseANDOperator(TreeNode* t);
    TreeNode* syEqualityExpression(TreeNode* t);
    TreeNode* syEqualityOperator(TreeNode* t);
    TreeNode* syRelationalExpression(TreeNode* t);
    TreeNode* syRelationalOperator(TreeNode* t);
    TreeNode* syShiftExpression(TreeNode* t);
    TreeNode* syShiftOperator(TreeNode* t);
    TreeNode* syAdditiveExpression(TreeNode* t);
    TreeNode* syAdditiveOperator(TreeNode* t);
    TreeNode* syMultiplicativeExpression(TreeNode* t);
    TreeNode* syMultiplicativeOperator(TreeNode* t);
    TreeNode* syUnaryExpression(TreeNode* t);
    TreeNode* syUnaryOperator(TreeNode* t);
    TreeNode* syPostfixExpression(TreeNode* t);
    TreeNode* syPostfixOperator(TreeNode* t);
    TreeNode* syVariableStatement(TreeNode* t);
    TreeNode* syVariableDeclarationList(TreeNode* t);
    TreeNode* syVariableDeclaration(TreeNode* t);
    TreeNode* syInitialiser(TreeNode* t);
    TreeNode* syPrimaryExpression(TreeNode* t);
    TreeNode* syLiteral(TreeNode* t);
    TreeNode* syArrayLiteral(TreeNode* t);
    TreeNode* syElision(TreeNode* t);
	TreeNode* syProgram(TreeNode* t);
	TreeNode* syReturnStatement(TreeNode* t);
	TreeNode* syIfStatement(TreeNode* t);
};


#endif
