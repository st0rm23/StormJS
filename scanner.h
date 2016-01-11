#ifndef _SCANNER_H_
#define _SCANNER_H_

#include "global.h"


typedef enum {
			START, INOVER, INCOMMENT1, INCOMMENT21, INCOMMENT22,
			INNUM_HEX0, INNUM_HEX1, INNUM_HEX2,
			INNUM0, INNUM_FLOAT0, INNUM_FLOAT1, INNUM_FLOAT2, INNUM_FLOAT3,
			INID, INSTRING, INCHAR0, INCHAR1, INCHAR2,
			INAND, INOR, INXOR, INANGLE_LEFT, INANGLE_RIGHT, INEQUAL, INEXCL,
			INPLUS, INMINUS, INTIMES, INMOD,
			DONE, INERROR
		} StateType;


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
