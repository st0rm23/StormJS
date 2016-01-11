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

    TreeNode* syFunctionDeclaration();
    TreeNode* syIdentifier();
    TreeNode* syFormalParameterList();
    TreeNode* syFunctionBody();
    TreeNode* sySourceElements();
    TreeNode* sySourceElement();
    TreeNode* syStatement();
    TreeNode* syExpressionStatement();
    TreeNode* syExpression();
    TreeNode* syAssignmentExpression();
    TreeNode* syLeftHandSideExpression();
    TreeNode* syMemberExpression();
    TreeNode* syMemberExpressionPart();
    TreeNode* syCallExpressionPart();
    TreeNode* syArguments();
    TreeNode* syArgumentList();
    TreeNode* syFunctionExpression();
    TreeNode* syAssignmentOperator();
    TreeNode* syConditionalExpression();
    TreeNode* syLogicalORExpression();
    TreeNode* syLogicalOROperator();
    TreeNode* syLogicalANDExpression();
    TreeNode* syLogicalANDOperator();
    TreeNode* syBitwiseORExpression();
    TreeNode* syBitwiseOROperator();
    TreeNode* syBitwiseXORExpression();
    TreeNode* syBitwiseXOROperator();
    TreeNode* syBitwiseANDExpression();
    TreeNode* syBitwiseANDOperator();
    TreeNode* syEqualityExpression();
    TreeNode* syEqualityOperator();
    TreeNode* syRelationalExpression();
    TreeNode* syRelationalOperator();
    TreeNode* syShiftExpression();
    TreeNode* syShiftOperator();
    TreeNode* syAdditiveExpression();
    TreeNode* syAdditiveOperator();
    TreeNode* syMultiplicativeExpression();
    TreeNode* syMultiplicativeOperator();
    TreeNode* syUnaryExpression();
    TreeNode* syUnaryOperator();
    TreeNode* syPostfixExpression();
    TreeNode* syPostfixOperator();
    TreeNode* Parser::syVariableStatement();
    TreeNode* Parser::syVariableDeclarationList();
    TreeNode* Parser::syVariableDeclaration();
    TreeNode* Parser::syInitialiser();
    TreeNode* Parser::syPrimaryExpression();
    TreeNode* Parser::syLiteral();
    TreeNode* Parser::syArrayLiteral();
    TreeNode* Parser::syElision();
};


#endif
