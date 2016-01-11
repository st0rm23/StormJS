#include "scanner.h"

Scanner::Scanner(FILE *file){
    stream = file;
    traceScan = true;
    lineno = 1;
    reservedChar = 0;
    haveReservedChar = false;
}

char Scanner::getNextChar(){
    if (haveReservedChar){
        haveReservedChar = false;
        return reservedChar;
    }
    else {
        if (reservedChar == '\n') lineno++;
        reservedChar = fgetc(stream);
        return reservedChar;
    }
}

void Scanner::ungetNextChar(){
haveReservedChar = true;
}

bool Scanner::isDigit(char ch){
    return '0' <= ch && ch <= '9';
}

bool Scanner::isAlpha(char ch){
    return ('a' <= ch && ch <= 'z')
        || ('A' <= ch && ch <= 'Z')
        || (ch == '$')
        || (ch == '_');
}

bool Scanner::isWhiteSpace(char ch){
    return ch == '\n'
        || ch == '\r'
        || ch == '\t'
        || ch == ' ';
}

Token Scanner::getToken(){
    Token token;
    StateType state = START;
    bool save;
    while (state != DONE) {
        char c = getNextChar();
        save = true;

        switch(state){
            case START:
                if (isWhiteSpace(c)) {
                    save = false;
                    state = START;
                }
                else if (c == '0') state = INNUM_HEX0;
                else if ('1' <= c && c <= '9') state = INNUM0;
                else if (isAlpha(c)) state = INID;
                else if (c == '\"') {save = false; state = INSTRING;}
                else if (c == '\'') {save = false; state = INCHAR0;}
                else if (c == '/') state = INOVER;
                else if (c == '&') state = INAND;
                else if (c == '|') state = INOR;
                else if (c == '^') state = INXOR;
                else if (c == '<') state = INANGLE_LEFT;
                else if (c == '>') state = INANGLE_RIGHT;
                else if (c == '=') state = INEQUAL;
                else if (c == '!') state = INEXCL;
                else if (c == '+') state = INPLUS;
                else if (c == '-') state = INMINUS;
                else if (c == '*') state = INTIMES;
                else if (c == '%') state = INMOD;
                else {
                    state = DONE;
                    switch (c){
                    case EOF:
                        save = false;
                        token.type = ENDFILE;
                        break;
                    case '.':
                        token.type = DOT;
                        break;
                    case '(':
                        token.type = LPSMALL;
                        break;
                    case ')':
                        token.type = RPSMALL;
                        break;
                    case '[':
                        token.type = LPMID;
                        break;
                    case ']':
                        token.type = RPBIG;
                        break;
                    case '{':
                        token.type = LPBIG;
                        break;
                    case '}':
                        token.type = RPBIG;
                        break;
                    case ';':
                        token.type = SEMI;
                        break;
                    case ',':
                        token.type = COMMA;
                        break;
                    case ':':
                        token.type = COLON;
                        break;
                    case '?':
                        token.type = QUES;
                        break;
                    case '~':
                        token.type = REVERSE;
                        break;
                    default:
                        state = INERROR;
                        token.type = ERROR;
                        break;
                    }
                }
                break;
            case INOVER:
                switch (c) {
                    case '/':
                        save = false;
                        token.value.clear();
                        state = INCOMMENT1;
                        break;
                    case '*':
                        save = false;
                        token.value.clear();
                        state = INCOMMENT21;
                        break;
                    case '=':
                        state = DONE;
                        token.type = OASSIGN;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = OVER;
                        break;
                }
                break;
            case INCOMMENT1:
                save = false;
                if (c == '\n') state = START;
                else state = INCOMMENT1;
                break;
            case INCOMMENT21:
                save = false;
                if (c == '*') state = INCOMMENT22;
                else state = INCOMMENT21;
                break;
            case INCOMMENT22:
                save = false;
                if (c == '/') state = START;
                else state = INCOMMENT21;
                break;
            case INNUM_HEX0:
                if (isDigit(c)) state = INNUM0;
                else if (c == 'X' || c == 'x') state = INNUM_HEX1;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = INTEGER;
                }
                break;
            case INNUM_HEX1:
                if (isDigit(c) || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F')
                    state = INNUM_HEX2;
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INNUM_HEX2:
                if (isDigit(c) || 'a' <= c && c <= 'f' || 'A' <= c && c <= 'F')
                    state = INNUM_HEX2;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = INTEGER;
                }
                break;
            case INNUM0:
                if (isDigit(c)) state = INNUM0;
                else if (c == '.') state = INNUM_FLOAT0;
                else if (c == 'e' || c == 'E') state = INNUM_FLOAT2;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = INTEGER;
                }
                break;
            case INNUM_FLOAT0:
                if (isDigit(c)) state = INNUM_FLOAT1;
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INNUM_FLOAT1:
                if (isDigit(c)) state = INNUM_FLOAT1;
                else if (c == 'e' || c == 'E') state = INNUM_FLOAT2;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = DECIMAL;
                }
                break;
            case INNUM_FLOAT2:
                if (isDigit(c)) state = INNUM_FLOAT3;
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INNUM_FLOAT3:
                if (isDigit(c)) state = INNUM_FLOAT3;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = DECIMAL;
                }
                break;
            case INID:
                if (isAlpha(c)) state = INID;
                else {
                    save = false;
                    ungetNextChar();
                    state = DONE;
                    token.type = ID;
                }
                break;
            case INSTRING:
                if (c != '\"') state = INSTRING;
                else {
                    save = false;
                    state = DONE;
                    token.type = STRING;
                }
                break;
            case INCHAR0:
                if (c == '\\') state = INCHAR1;
                else if (c != '\\' && c != '\'') state = INCHAR2;
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INCHAR1:
                if (c != '\'' && c != '\\') state = INCHAR2;
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INCHAR2:
                if (c == '\'') {
                    save = false;
                    state = DONE;
                    token.type = CHARACTER;
                }
                else {
                    save = false;
                    state = INERROR;
                }
                break;
            case INAND:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = ANDASSIGN;
                        break;
                    case '&':
                        state = DONE;
                        token.type = ANDAND;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = AND;
                        break;
                }
                break;
            case INOR:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = ORASSIGN;
                        break;
                    case '|':
                        state = DONE;
                        token.type = OROR;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = OR;
                        break;
                }
                break;
            case INXOR:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = XORASSIGN;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = XOR;
                        break;
                }
                break;
            case INANGLE_LEFT:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = LESSEUQAL;
                        break;
                    case '<':
                        state = DONE;
                        token.type = LSHIFT;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = LESS;
                        break;
                }
                break;
            case INANGLE_RIGHT:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = GREATQUAL;
                        break;
                    case '>':
                        state = DONE;
                        token.type = RSHIFT;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = GREAT;
                        break;
                }
                break;
            case INEQUAL:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = EQUAL;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = ASSIGN;
                        break;
                }
                break;
            case INEXCL:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = UNEQUAL;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = NOT;
                        break;
                }
                break;
            case INPLUS:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = PASSIGN;
                        break;
                    case '+':
                        state = DONE;
                        token.type = INCREASE;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = PLUS;
                        break;
                }
                break;
            case INMINUS:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = MINUSASSIGN;
                        break;
                    case '-':
                        state = DONE;
                        token.type = DECREASE;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = MINUS;
                        break;
                }
                break;
            case INTIMES:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = TASSIGN;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = TIMES;
                        break;
                }
                break;
            case INMOD:
                switch (c) {
                    case '=':
                        state = DONE;
                        token.type = MODASSIGN;
                        break;
                    default:
                        save = false;
                        ungetNextChar();
                        state = DONE;
                        token.type = MOD;
                        break;
                }
                break;
            case DONE:
            case INERROR:
            default:
                fprintf(stderr, "%s\n", "error");
                state = DONE;
                token.type = ERROR;
        }
        if (save) token.value += c;
        if (state == DONE) {
            if (token.type == ID)
                token.type = resevedLookup(token.value);
        }
    }
    if (traceScan) {
        static int preline = 0;
        if (lineno != preline)
			fprintf(stderr, "%s%d:\t", preline == 0 ? "" : "\n", lineno);
        preline = lineno;
        printToken(token);
    }
    return token;
}

TokenType Scanner::resevedLookup(const string &str){
    if (str == "break") return BREAK;
    if (str == "case") return CASE;
    if (str == "catch") return CATCH;
    if (str == "continue") return CONTINUE;
    if (str == "default") return DEFAULT;
    if (str == "delete") return DELETE;
    if (str == "do") return DO;
    if (str == "finally") return FINALLY;
    if (str == "for") return FOR;
    if (str == "function") return FUNCTION;
    if (str == "if") return IF;
    if (str == "in") return IN;
    if (str == "instanceof") return INSTANCEOF;
    if (str == "new") return NEW;
    if (str == "return") return RETURN;
    if (str == "switch") return SWITCH;
    if (str == "this") return THIS;
    if (str == "throw") return THROW;
    if (str == "try") return TRY;
    if (str == "typeof") return TYPEOF;
    if (str == "var") return VAR;
    if (str == "void") return VOID;
    if (str == "while") return WHILE;
    if (str == "with") return WITH;
    if (str == "abstract") return ABSTRACT;
    if (str == "boolean") return BOOLEAN;
    if (str == "type") return TYPE;
    if (str == "char") return CHAR;
    if (str == "class") return CLASS;
    if (str == "const") return CONST;
    if (str == "debugger") return DEBUGGER;
    if (str == "double") return DOUBLE;
    if (str == "enum") return ENUM;
    if (str == "export") return EXPORT;
    if (str == "extends") return EXTENDS;
    if (str == "final") return FINAL;
    if (str == "float") return FLOAT;
    if (str == "goto") return GOTO;
    if (str == "implements") return IMPLEMENTS;
    if (str == "import") return IMPORT;
    if (str == "int") return INT;
    if (str == "interface") return INTERFACE;
    if (str == "long") return LONG;
    if (str == "mative") return MATIVE;
    if (str == "package") return PACKAGE;
    if (str == "private") return PRIVATE;
    if (str == "protected") return PROTECTED;
    if (str == "public") return PUBLIC;
    if (str == "shrot") return SHROT;
    if (str == "static") return STATIC;
    if (str == "super") return SUPER;
    if (str == "synchronized") return SYNCHRONIZED;
    if (str == "throws") return THROWS;
    if (str == "transient") return TRANSIENT;
    if (str == "volatile") return VOLATILE;
    if (str == "true") return VBOOLEAN;
    if (str == "false") return VBOOLEAN;
    if (str == "null") return EMPTY;
    if (str == "undefined") return EMPTY;
    if (str == "NaN") return EMPTY;
    return ID;
}

int Scanner::getLine(){
	return lineno;
};

char Scanner::getNowChar(){
	return reservedChar;
};

void Scanner::printToken(const Token &token){
   // fprintf(stderr, "<%s:%s>", getTokenName(token.type).c_str(), token.value.c_str());
	fprintf(stderr, "%s ", token.value.c_str());
}

string Scanner::getTokenName(TokenType type){
    switch (type) {
        case ID: return "ID";
        case INTEGER: return "INTEGER";
        case DECIMAL: return "DECIMAL";
        case HEX: return "HEX";
        case STRING: return "STRING";
		case CHARACTER: return "CHARACTER";
        case LESS: return "LESS";
        case GREAT: return "GREAT";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case TIMES: return "TIMES";
        case NOT: return "NOT";
        case OVER: return "OVER";
        case MOD: return "MOD";
        case OR: return "OR";
        case XOR: return "XOR";
        case AND: return "AND";
        case OROR: return "OROR";
        case ANDAND: return "ANDAND";
        case EQUAL: return "EQUAL";
        case UNEQUAL: return "UNEQUAL";
        case LSHIFT: return "LSHIFT";
        case RSHIFT: return "RSHIFT";
        case LESSEUQAL: return "LESSEUQAL";
        case GREATQUAL: return "GREATQUAL";
        case INCREASE: return "INCREASE";
        case DECREASE: return "DECREASE";
        case PASSIGN: return "PASSIGN";
        case MINUSASSIGN: return "MINUSASSIGN";
        case TASSIGN: return "TASSIGN";
        case OASSIGN: return "OASSIGN";
        case MODASSIGN: return "MODASSIGN";
        case ORASSIGN: return "ORASSIGN";
        case XORASSIGN: return "XORASSIGN";
        case ANDASSIGN: return "ANDASSIGN";
        case ASSIGN: return "ASSIGN";
        case LPBIG: return "LPBIG";
        case RPBIG: return "RPBIG";
        case LPSMALL: return "LPSMALL";
        case RPSMALL: return "RPSMALL";
        case LPMID: return "LPMID";
        case RPMID: return "RPMID";
        case SEMI: return "SEMI";
        case DOT: return "DOT";
        case COMMA: return "COMMA";
        case COLON: return "COLON";
        case QUES: return "QUES";
        case ENDFILE: return "ENDFILE";
        case ERROR: return "ERROR";

        case BOOLEAN: return "BOOLEAN";
        case EMPTY: return "EMPTY";
        case BREAK: return "BREAK";
        case CASE: return "CASE";
        case CATCH: return "CATCH";
        case CONTINUE: return "CONTINUE";
        case DEFAULT: return "DEFAULT";
        case DELETE: return "DELETE";
        case DO: return "DO";
        case FINALLY: return "FINALLY";
        case FOR: return "FOR";
        case FUNCTION: return "FUNCTION";
        case IF: return "IF";
        case IN: return "IN";
        case INSTANCEOF: return "INSTANCEOF";
        case NEW: return "NEW";
        case RETURN: return "RETURN";
        case SWITCH: return "SWITCH";
        case THIS: return "THIS";
        case THROW: return "THROW";
        case TRY: return "TRY";
        case TYPEOF: return "TYPEOF";
        case VAR: return "VAR";
        case VOID: return "VOID";
        case WHILE: return "WHILE";
        case WITH: return "WITH";
        case ABSTRACT: return "ABSTRACT";
        case VBOOLEAN: return "VBOOLEAN";
        case TYPE: return "TYPE";
        case CHAR: return "CHAR";
        case CLASS: return "CLASS";
        case CONST: return "CONST";
        case DEBUGGER: return "DEBUGGER";
        case DOUBLE: return "DOUBLE";
        case ENUM: return "ENUM";
        case EXPORT: return "EXPORT";
        case EXTENDS: return "EXTENDS";
        case FINAL: return "FINAL";
        case FLOAT: return "FLOAT";
        case GOTO: return "GOTO";
        case IMPLEMENTS: return "IMPLEMENTS";
        case IMPORT: return "IMPORT";
        case INT: return "INT";
        case INTERFACE: return "INTERFACE";
        case LONG: return "LONG";
        case MATIVE: return "MATIVE";
        case PACKAGE: return "PACKAGE";
        case PRIVATE: return "PRIVATE";
        case PROTECTED: return "PROTECTED";
        case PUBLIC: return "PUBLIC";
        case SHROT: return "SHROT";
        case STATIC: return "STATIC";
        case SUPER: return "SUPER";
        case SYNCHRONIZED: return "SYNCHRONIZED";
        case THROWS: return "THROWS";
        case TRANSIENT: return "TRANSIENT";
        case VOLATILE: return "VOLATILE";
    }
}
