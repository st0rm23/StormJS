#ifndef _GLOBAL_H_
#define _GLOBAL_H_

#include <string>
#include <cstdio>
#include <vector>

using namespace std;

typedef enum{
	FunctionDeclaration, Expression, Identifier, FormalParameterList, FunctionBody,
	SourceElements, SourceElement, Statement, ExpressionStatement, Elision,
	AssignmentExpression, LeftHandSideExpression, CallExpressionPart, Arguments,
	ArgumentList, MemberExpression, MemberExpressionPart, FunctionExpression,
	ConditionalExpression, LogicalORExpression, LogicalANDExpression, BitwiseORExpression,
	BitwiseXORExpression, BitwiseANDExpression, EqualityExpression, RelationalExpression,
	ShiftExpression, AdditiveExpression, MultiplicativeExpression, UnaryExpression,
	PostfixExpression, LogicalOROperator, LogicalANDOperator, BitwiseOROperator,
	BitwiseXOROperator, BitwiseANDOperator, EqualityOperator, RelationalOperator,
	ShiftOperator, AdditiveOperator, MultiplicativeOperator, UnaryOperator, PostfixOperator,
	AssignmentOperator ,VariableStatement, VariableDeclarationList, VariableDeclaration,
	Initialiser, PrimaryExpression, Literal, ArrayLiteral, Program, ReturnStatement,
	IfStatement,
} NodeKind;

typedef enum{
	/* book-kepping tokens */
	ENDFILE, ERROR,
	/* reserved words */
	BREAK, CASE, CATCH, CONTINUE, DEFAULT, DELETE, DO,
	FINALLY, FOR, FUNCTION, IF, IN, INSTANCEOF, NEW, RETURN, ELSE,
	SWITCH, THIS, THROW, TRY, TYPEOF, VAR, VOID, WHILE, WITH,
	ABSTRACT, BOOLEAN, TYPE, CHAR, CLASS, CONST, DEBUGGER, DOUBLE,
	ENUM, EXPORT, EXTENDS, FINAL, FLOAT, GOTO, IMPLEMENTS, IMPORT,
	INT, INTERFACE, LONG, MATIVE, PACKAGE, PRIVATE, PROTECTED, PUBLIC,
	SHROT, STATIC, SUPER, SYNCHRONIZED, THROWS, TRANSIENT, VOLATILE,
	/* muilticharacter tokens */
	ID, INTEGER, DECIMAL, HEX, STRING, VBOOLEAN, 
	INF, NANUMBER, UNDEFINED, NIL,
	/* spectial symbols */
	//<,  >,      +,     -,     *,    !     /,   %   |    ^    &    ||     &&
	LESS, GREAT, PLUS, MINUS, TIMES, NOT, OVER, MOD, OR, XOR, AND, OROR, ANDAND,
	//==,   !=,        <<,    >>,        <=,       >=,         ++       --
	EQUAL, UNEQUAL, LSHIFT, RSHIFT, LESSEUQAL, GREATQUAL, INCREASE, DECREASE,
	// +=      -=            *=      /=       %=         |=       ^=           &=
	PASSIGN, MINUSASSIGN, TASSIGN, OASSIGN, MODASSIGN, ORASSIGN, XORASSIGN, ANDASSIGN,
	// =       {	 }       (        )       [       ]     ;     .    ,      :     ？    ~
	ASSIGN, LPBIG, RPBIG, LPSMALL, RPSMALL, LPMID, RPMID, SEMI, DOT, COMMA, COLON, QUES, REVERSE

} TokenType;

struct Token
{
	TokenType type;
	string value;
};

class TreeNode{
public:
    NodeKind kind;
	TreeNode* parent;
    TreeNode* child;
    TreeNode* sibling;
    Token token;
    int line;

    TreeNode(int line, NodeKind kind, TreeNode* parent);
    ~TreeNode();
};


class ScannerException : public exception{};
class ParserException : public exception{};
class ExpressionException : public exception{};
class ExecutionException : public exception{};

#endif
