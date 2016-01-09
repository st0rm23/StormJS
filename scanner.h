#ifndef _SCANNER_H_
#define _SCANNER_H_

#include <string>
#include <cstdio>

using namespace std;

typedef enum{
	/* book-kepping tokens */
	ENDFILE, ERROR,
	/* reserved words */
	BREAK, CASE, CATCH, CONTINUE, DEFAULT, DELETE, DO,
	FINALLY, FOR, FUNCTION, IF, IN, INSTANCEOF, NEW, RETURN,
	SWITCH, THIS, THROW, TRY, TYPEOF, VAR, VOID, WHILE, WITH,
	ABSTRACT, BOOLEAN, TYPE, CHAR, CLASS, CONST, DEBUGGER, DOUBLE,
	ENUM, EXPORT, EXTENDS, FINAL, FLOAT, GOTO, IMPLEMENTS, IMPORT,
	INT, INTERFACE, LONG, MATIVE, PACKAGE, PRIVATE, PROTECTED, PUBLIC,
	SHROT, STATIC, SUPER, SYNCHRONIZED, THROWS, TRANSIENT, VOLATILE,
	/* muilticharacter tokens */
	ID, INTEGER, DECIMAL, HEX, STRING, CHARACTER,
	/* spectial symbols */
	//<,  >,      +,     -,     *,    !     /,   %   |    ^    &    ||     &&
	LESS, GREAT, PLUS, MINUS, TIMES, NOT, OVER, MOD, OR, XOR, AND, OROR, ANDAND,
	//==,   !=,        <<,    >>,        <=,       >=,         ++       --
	EQUAL, UNEQUAL, LSHIFT, RSHIFT, LESSEUQAL, GREATQUAL, INCREASE, DECREASE,
	// +=      -=            *=      /=       %=         |=       ^=           &=
	PASSIGN, MINUSASSIGN, TASSIGN, OASSIGN, MODASSIGN, ORASSIGN, XORASSIGN, ANDASSIGN,
	// =       {	 }       (        )       [       ]     ;     .    ,      :
	ASSIGN,  LPBIG, RPBIG, LPSMALL, RPSMALL, LPMID, RPMID, SEMI, DOT, COMMA, COLON

} TokenType;

typedef enum {
			START, INOVER, INCOMMENT1, INCOMMENT21, INCOMMENT22,
			INNUM_HEX0, INNUM_HEX1, INNUM_HEX2,
			INNUM0, INNUM_FLOAT0, INNUM_FLOAT1, INNUM_FLOAT2, INNUM_FLOAT3,
			INID, INSTRING, INCHAR0, INCHAR1, INCHAR2,
			INAND, INOR, INXOR, INANGLE_LEFT, INANGLE_RIGHT, INEQUAL, INEXCL,
			INPLUS, INMINUS, INTIMES, INMOD,
			DONE, INERROR
		} StateType;

struct Token
{
	TokenType type;
	string value;
};


class Scanner{
private:
    bool traceScan;
    int lineno;
    FILE *stream;
    char reservedChar;
    bool haveReservedChar;

public:
    Scanner(FILE *file);
    char getNextChar();
    void ungetNextChar();
    bool isDigit(char ch);
    bool isAlpha(char ch);
    bool isWhiteSpace(char ch);
    Token getToken();
    TokenType resevedLookup(const string &str);
	void printToken(const Token &token);
    string getTokenName(TokenType type);
	int getLine();
	char getNowChar();
};

#endif
