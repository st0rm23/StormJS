#include "parser.h"

Parser::Parser(FILE* stream){
    scanner = new Scanner(stream);
    token = scanner->getToken();
	reservedLeftExpressions.clear();
}

Parser::~Parser(){
    delete scanner;
}

void Parser::match(const TokenType tokenType){
    if (token.type == tokenType) token = scanner->getToken();
	else throw ParserException();
}

void Parser::printError(){
	if (token.type != ERROR){
		int line = scanner->getLine();
		fprintf(stderr, "\n>>> Syntax error at line %d: ", line);
		fprintf(stderr, "unexpected token -> ");
		scanner->printToken(token);
		fprintf(stderr, "\n");
	}
	else {
		int line = scanner->getLine();
		fprintf(stderr, "\n>>> Scanner error at line %d: ", line);
		fprintf(stderr, "unexpected character after token -> ");
		scanner->printToken(token);
		fprintf(stderr, "\n");
	}
}


TreeNode* Parser::syFunctionDeclaration(){
    TreeNode* t = new TreeNode(scanner->getLine(), FunctionDeclaration);
    TreeNode* c = NULL;
    match(FUNCTION);
    c = t->child = syIdentifier();
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = c->sibling = syFormalParameterList();
    match(RPSMALL);
    c = c->sibling = syFunctionBody();
    return t;
}

TreeNode* Parser::syIdentifier(){
    TreeNode* t = new TreeNode(scanner->getLine(), Identifier);
    t->token = token;
    TreeNode* c = NULL;
    match(ID);
    return t;
}

TreeNode* Parser::syFormalParameterList(){
    TreeNode* t = new TreeNode(scanner->getLine(), FormalParameterList);
    TreeNode* c = NULL;
    c = t->child = syIdentifier();
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syIdentifier();
    }
    return t;
}

TreeNode* Parser::syFunctionBody(){
    TreeNode* t = new TreeNode(scanner->getLine(), FunctionBody);
    TreeNode* c = NULL;
    match(LPBIG);
    if (token.type != RPBIG)
        c = t->child = sySourceElements();
    match(RPBIG);
    return t;
}

TreeNode* Parser::sySourceElements(){
    TreeNode* t = new TreeNode(scanner->getLine(), SourceElements);
    TreeNode* c = NULL;
    c = t->child = sySourceElement();
    while (token.type != RPBIG)
        c = c->sibling = sySourceElement();
    return t;
}

TreeNode* Parser::sySourceElement(){
    TreeNode* t = new TreeNode(scanner->getLine(), SourceElement);
    TreeNode* c = NULL;
    if (token.type == FUNCTION)
        c = t->child = syFunctionDeclaration();
    else
        c = t->child = syStatement();
    return t;
}

TreeNode* Parser::syStatement(){   //incomplete
    TreeNode* t = new TreeNode(scanner->getLine(), Statement);
    TreeNode* c = NULL;
    switch (token.type){
        case VAR:
            c = t->child = syVariableStatement();
            break;
        default:
            c = t->child = syExpressionStatement();
    }
    return t;
}

TreeNode* Parser::syExpressionStatement(){
    TreeNode* t = new TreeNode(scanner->getLine(), ExpressionStatement);
    TreeNode* c = NULL;
    c = t->child = syExpression();
    if (token.type == SEMI) match(SEMI);
    return t;
}

TreeNode* Parser::syExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), Expression);
    TreeNode* c = NULL;
    c = t->child = syAssignmentExpression();
	while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syAssignmentExpression();
    }
    return t;
}

TreeNode* Parser::syAssignmentExpression(){       //AssignmentExpression -> ( LeftHandSideExpression AssignmentOperator AssignmentExpression ) | ( ConditionalExpression )
    TreeNode* t = new TreeNode(scanner->getLine(), AssignmentExpression);
    TreeNode* c = NULL;
	reservedLeftExpressions.push_back(syLeftHandSideExpression());

	switch (token.type){
		case ASSIGN:
		case TASSIGN:
		case MODASSIGN:
		case PASSIGN:
		case MINUSASSIGN:
		case ANDASSIGN:
		case OASSIGN:
		case XORASSIGN:
		case ORASSIGN:
			c = t->child = reservedLeftExpressions.back();
			reservedLeftExpressions.pop_back();
			c = c->sibling = syAssignmentOperator();
			c = c->sibling = syAssignmentExpression();
			break;
		default:
			c = t->child = syConditionalExpression();
			break;
	}
    return t;
}

TreeNode* Parser::syLeftHandSideExpression(){      //LeftHandSideExpression -> MemberExpression [ Arguments ( CallExpressionPart )* ];
    TreeNode* t = new TreeNode(scanner->getLine(), LeftHandSideExpression);
    TreeNode* c = NULL;
    c = t->child = syMemberExpression();
    if (token.type == LPSMALL){
        c = c->sibling = syArguments();
        while (token.type == LPSMALL || token.type == LPMID || token.type == DOT)
            c = c->sibling = syCallExpressionPart();
    }
    return t;
}

TreeNode* Parser::syMemberExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), MemberExpression);
    TreeNode* c = NULL;

    if (token.type == NEW){                 //incomplete

    }
    else{
        if (token.type == FUNCTION)
            c = t->child = syFunctionExpression();
        else
            c = t->child = syPrimaryExpression();
        while (token.type == LPMID || token.type == DOT){
            c = c->sibling = syMemberExpressionPart();
        }
    }
    return t;
}

TreeNode* Parser::syMemberExpressionPart(){
	TreeNode* t = new TreeNode(scanner->getLine(), MemberExpressionPart);
    TreeNode* c = NULL;

    if (token.type == LPMID){
        match(LPMID);
        c = t->child = syExpression();
        match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier();
    }
    return t;
}

TreeNode* Parser::syCallExpressionPart(){
    TreeNode* t = new TreeNode(scanner->getLine(), CallExpressionPart);
    TreeNode* c = NULL;
    if (token.type == LPSMALL)
        c = t->child = syArguments();
    else if (token.type == LPMID) {
            match(LPMID);
            c = t->child = syExpression();
            match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier();
    }
    return t;
}

TreeNode* Parser::syArguments(){
    TreeNode* t = new TreeNode(scanner->getLine(), Arguments);
    TreeNode* c = NULL;
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = t->child = syArgumentList();
    match(RPSMALL);
    return t;
}

TreeNode* Parser::syArgumentList(){
    TreeNode* t = new TreeNode(scanner->getLine(), ArgumentList);
    TreeNode* c = NULL;
    c = t->child = syAssignmentExpression();
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syAssignmentExpression();
    }
    return t;
}

TreeNode* Parser::syFunctionExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), FunctionExpression);
    TreeNode* c = NULL;
    match(FUNCTION);
    if (token.type == ID) c = t->child = syIdentifier();
    else c = t->child = new TreeNode(scanner->getLine(), Identifier);
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = t->child = syFormalParameterList();
    match(RPSMALL);
    c = c->sibling = syFunctionBody();
    return t;
}

TreeNode* Parser::syAssignmentOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), AssignmentOperator);
    TreeNode* c = NULL;
	t->token = token;
    switch (token.type){
        case ASSIGN:
			match(ASSIGN);
			break;
		case TASSIGN:
			match(TASSIGN);
			break;
		case MODASSIGN:
			match(MODASSIGN);
			break;
		case PASSIGN:
			match(PASSIGN);
			break;
		case MINUSASSIGN:
			match(MINUSASSIGN);
			break;
		case ANDASSIGN:
			match(ANDASSIGN);
			break;
		case OASSIGN:
			match(OASSIGN);
			break;
		case XORASSIGN:
			match(XORASSIGN);
			break;
		case ORASSIGN:
			match(ORASSIGN);
			break;
        default:
            throw ParserException();
    }
    return t;
}

TreeNode* Parser::syConditionalExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), ConditionalExpression);
    TreeNode* c = NULL;
    c = t->child = syLogicalORExpression();
    if (token.type == QUES) {
        match(QUES);
        c = c->sibling = syAssignmentExpression();
        match(COLON);
        c = c->sibling = syAssignmentExpression();
    }
    return t;
}

TreeNode* Parser::syLogicalORExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), LogicalORExpression);
    TreeNode* c = NULL;
    c = t->child = syLogicalANDExpression();
    while (token.type == OROR){
        c = c->sibling = syLogicalOROperator();
        c = c->sibling = syLogicalANDExpression();
    }
    return t;
}

TreeNode* Parser::syLogicalOROperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), LogicalOROperator);
    TreeNode* c = NULL;
	t->token = token;
    match(OROR);
    return t;
}

TreeNode* Parser::syLogicalANDExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), LogicalANDExpression);
    TreeNode* c = NULL;
    c = t->child = syBitwiseORExpression();
    while (token.type == ANDAND){
        c = c->sibling = syLogicalANDExpression();
        c = c->sibling = syBitwiseORExpression();
    }
    return t;
}

TreeNode* Parser::syLogicalANDOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), LogicalANDOperator);
    TreeNode* c = NULL;
	t->token = token;
    match(ANDAND);
    return t;
}

TreeNode* Parser::syBitwiseORExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseORExpression);
    TreeNode* c = NULL;
    c = t->child = syBitwiseXORExpression();
    while (token.type == OR){
        c = c->sibling = syBitwiseOROperator();
        c = c->sibling = syBitwiseXORExpression();
    }
    return t;
}

TreeNode* Parser::syBitwiseOROperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseOROperator);
	t->token = token;
    match(OR);
    return t;
}

TreeNode* Parser::syBitwiseXORExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseXORExpression);
    TreeNode* c = NULL;
    c = t->child = syBitwiseANDExpression();
    while (token.type == XOR){
        c = c->sibling = syBitwiseXOROperator();
        c = c->sibling = syBitwiseANDExpression();
    }
    return t;
}

TreeNode* Parser::syBitwiseXOROperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseXOROperator);
	t->token = token;
    match(XOR);
    return t;
}

TreeNode* Parser::syBitwiseANDExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseANDExpression);
    TreeNode* c = NULL;
    c = t->child = syEqualityExpression();
    while (token.type == AND){
        c = c->sibling = syBitwiseANDOperator();
        c = c->sibling = syEqualityExpression();
    }
    return t;
}

TreeNode* Parser::syBitwiseANDOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), BitwiseANDOperator);
	t->token = token;
    match(AND);
    return t;
}


TreeNode* Parser::syEqualityExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), EqualityExpression);
    TreeNode* c = NULL;
    c = t->child = syRelationalExpression();
    while (token.type == EQUAL || token.type == UNEQUAL){  //incomplete
        c = c->sibling = syEqualityOperator();
        c = c->sibling = syRelationalExpression();
    }
    return t;
}

TreeNode* Parser::syEqualityOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), EqualityOperator);
	t->token = token;
    if (token.type == EQUAL) match(EQUAL);
    else match(UNEQUAL);
    return t;
}

TreeNode* Parser::syRelationalExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), RelationalExpression);
    TreeNode* c = NULL;
    c = t->child = syShiftExpression();
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case LESS:
            case GREAT:
            case LESSEUQAL:
            case GREATQUAL:
            case INSTANCEOF:
            case IN:
                c = c->sibling = syRelationalOperator();
                    c = c->sibling = syShiftExpression();
                break;
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syRelationalOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), RelationalOperator);
	t->token = token;
    switch (token.type) {
        case LESS:
            match(LESS);
            break;
        case GREAT:
            match(GREAT);
            break;
        case LESSEUQAL:
            match(LESSEUQAL);
            break;
        case GREATQUAL:
            match(GREATQUAL);
            break;
        case INSTANCEOF:
            match(INSTANCEOF);
            break;
        case IN:
            match(IN);
            break;
        default:
            throw ParserException();
            break;
    }
    return t;
}

TreeNode* Parser::syShiftExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), ShiftExpression);
    TreeNode* c = NULL;
    c = t->child = syAdditiveExpression();
    bool yes = true;;
    while (yes){
        switch (token.type) {           //incomplete
            case LSHIFT:
            case RSHIFT:
                c = c->sibling = syShiftOperator();
                c = c->sibling = syAdditiveExpression();
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syShiftOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), ShiftOperator);
	t->token = token;
    switch (token.type) {
        case LSHIFT:
            match(LSHIFT);
            break;
        case RSHIFT:
            match(RSHIFT);
            break;
        default:
            throw ParserException();
            break;
    }
    return t;
}

TreeNode* Parser::syAdditiveExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), AdditiveExpression);
    TreeNode* c = NULL;
    c = t->child = syMultiplicativeExpression();
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case PLUS:
            case MINUS:
                c = c->sibling = syAdditiveOperator();
                c = c->sibling = syMultiplicativeExpression();
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syAdditiveOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), AdditiveOperator);
	t->token = token;
    switch (token.type) {
        case PLUS:
            match(PLUS);
            break;
        case MINUS:
            match(MINUS);
            break;
        default:
            throw ParserException();
            break;
    }
    return t;
}

TreeNode* Parser::syMultiplicativeExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), MultiplicativeExpression);
    TreeNode* c = NULL;
    c = t->child = syUnaryExpression();
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case TIMES:
            case OVER:
            case MOD:
                c = c->sibling = syMultiplicativeOperator();
                c = c->sibling = syUnaryExpression();
            default:
                yes = false;
                break;
        }
    }
    return t;
}


TreeNode*  Parser::syMultiplicativeOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), MultiplicativeOperator);
	t->token = token;
    switch (token.type) {
        case TIMES:
            match(TIMES);
            break;
        case OVER:
            match(OVER);
            break;
        case MOD:
            match(MOD);
            break;
        default:
            throw ParserException();
            break;
    }
    return t;
}

TreeNode*  Parser::syUnaryExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), UnaryExpression);
    TreeNode* c = NULL;
	bool yes = true;
    switch (token.type) {
        case DELETE:
        case VOID:
        case TYPEOF:
        case INCREASE:
        case DECREASE:
        case PLUS:
        case MINUS:
        case REVERSE:
        case NOT:
            c = t->child = syUnaryOperator();
            c = c->sibling = syUnaryExpression();
            while (yes){
                switch (token.type) {
                    case DELETE:
                    case VOID:
                    case TYPEOF:
                    case INCREASE:
                    case DECREASE:
                    case PLUS:
                    case MINUS:
                    case REVERSE:
                    case NOT:
                        c = c->sibling = syUnaryOperator();
                        c = c->sibling = syUnaryExpression();
                    default:
                        yes = false;
                        break;
                }
            }
            break;
        default:
            c = t->child = syPostfixExpression();
            break;
    }
    return t;
}


TreeNode*  Parser::syUnaryOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), UnaryOperator);
	t->token = token;
    switch (token.type) {
        case DELETE:
            match(DELETE);
            break;
        case VOID:
            match(VOID);
            break;
        case TYPEOF:
            match(TYPEOF);
            break;
        case INCREASE:
            match(INCREASE);
            break;
        case DECREASE:
            match(DECREASE);
            break;
        case PLUS:
            match(PLUS);
            break;
        case MINUS:
            match(MINUS);
            break;
        case REVERSE:
            match(REVERSE);
            break;
        case NOT:
            match(NOT);
            break;
        default:
            throw ParserException();
            break;
    }
    return t;
}


TreeNode*  Parser::syPostfixExpression(){
	TreeNode* t = new TreeNode(scanner->getLine(), PostfixExpression);
	TreeNode* c = NULL;
	if (reservedLeftExpressions.empty())
		c = t->child = syLeftHandSideExpression();
	else{
		c = t->child = reservedLeftExpressions.back();
		reservedLeftExpressions.pop_back();
	}
    if (token.type == INCREASE || token.type == DECREASE)
        c = c->sibling = syPostfixOperator();
    return t;
}

TreeNode* Parser::syPostfixOperator(){
    TreeNode* t = new TreeNode(scanner->getLine(), PostfixOperator);
    t->token = token;
    if (token.type == INCREASE) match(INCREASE);
    else match(DECREASE);
    return t;
}

TreeNode* Parser::syVariableStatement(){
    TreeNode* t = new TreeNode(scanner->getLine(), VariableStatement);
    TreeNode* c = NULL;
    match(VAR);
    c = t->child = syVariableDeclarationList();
    if (token.type == SEMI)
        match(SEMI);
    return t;
}

TreeNode* Parser::syVariableDeclarationList(){
    TreeNode* t = new TreeNode(scanner->getLine(), VariableDeclarationList);
    TreeNode* c = NULL;
    c = t->child = syVariableDeclaration();
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syVariableDeclaration();
    }
    return t;
}

TreeNode* Parser::syVariableDeclaration(){
    TreeNode* t = new TreeNode(scanner->getLine(), VariableDeclaration);
    TreeNode* c = NULL;
    c = t->child = syIdentifier();
    if (token.type == ASSIGN)
        c = c->sibling = syInitialiser();
    return t;
}

TreeNode* Parser::syInitialiser(){
    TreeNode* t = new TreeNode(scanner->getLine(), Initialiser);
    TreeNode* c = NULL;
    match(ASSIGN);
    c = t->child = syAssignmentExpression();
    return t;
}

TreeNode* Parser::syPrimaryExpression(){
    TreeNode* t = new TreeNode(scanner->getLine(), PrimaryExpression);
    TreeNode* c = NULL;
    switch (token.type) {
        case ID:
            c = t->child = syIdentifier();
            break;
        case LPSMALL:
            match(LPSMALL);
            c = t->child = syExpression();
            match(RPSMALL);
            break;
        case INTEGER:
        case DECIMAL:
        case HEX:
        case STRING:
        case CHARACTER:
        case VBOOLEAN:
        case EMPTY:
            c = t->child = syLiteral();
            break;
        case LPMID:
            c = t->child = syArrayLiteral();
            break;
        case THIS:
            //match(THIS);          //incomplete
            //break;
        case LPBIG:
            //c = t->child = syObjectLiteral();
            //break;
        default:
            throw ParserException();
    }
    return t;
}

TreeNode* Parser::syLiteral(){
    TreeNode* t = new TreeNode(scanner->getLine(), Literal);
    TreeNode* c = NULL;
    t->token = token;
    switch (token.type) {
        case INTEGER:
			match(INTEGER);
			break;
		case DECIMAL:
			match(DECIMAL);
			break;
		case HEX:
			match(HEX);
			break;
		case STRING:
			match(STRING);
			break;
		case CHARACTER:
			match(CHARACTER);
			break;
		case VBOOLEAN:
			match(VBOOLEAN);
			break;
		case EMPTY:
			match(EMPTY);
            break;
        default:
            throw ParserException();
    }
    return t;
}

TreeNode* Parser::syArrayLiteral(){      //ArrayLiteral -> '[' Elision ?      ']'
    TreeNode* t = new TreeNode(scanner->getLine(), ArrayLiteral);
    TreeNode* c = NULL;
    match(LPMID);
    while (token.type != RPMID){
        if (token.type == COMMA)
            if (t->child == NULL) c = t->child = syElision();
            else c = c->sibling = syElision();
        else {
            if (t->child == NULL) c = t->child = syAssignmentExpression();
            else c = c->sibling = syAssignmentExpression();
            if (token.type != ']') match(COMMA);
        }
    }
    match(RPMID);
    return t;
}

TreeNode* Parser::syElision(){
    TreeNode* t = new TreeNode(scanner->getLine(), Elision);
    TreeNode* c = NULL;
    t->token.value = "";
	t->token.type = COMMA;
	match(COMMA);
	t->token.value += ',';
    while (token.type == COMMA){
		t->token.value += ',';
        match(COMMA);
    }
    return t;
}