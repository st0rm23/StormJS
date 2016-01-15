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


TreeNode* Parser::syFunctionDeclaration(TreeNode* t){
    t = new TreeNode(scanner->getLine(), FunctionDeclaration, t);
    TreeNode* c = NULL;
    match(FUNCTION);
    c = t->child = syIdentifier(t);
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = c->sibling = syFormalParameterList(t);
    match(RPSMALL);
    c = c->sibling = syFunctionBody(t);
    return t;
}

TreeNode* Parser::syIdentifier(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Identifier, t);
    t->token = token;
    TreeNode* c = NULL;
    match(ID);
    return t;
}

TreeNode* Parser::syFormalParameterList(TreeNode* t){
	t = new TreeNode(scanner->getLine(), FormalParameterList, t);
    TreeNode* c = NULL;
    c = t->child = syIdentifier(t);
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syIdentifier(t);
    }
    return t;
}

TreeNode* Parser::syFunctionBody(TreeNode* t){
	t = new TreeNode(scanner->getLine(), FunctionBody, t);
    TreeNode* c = NULL;
    match(LPBIG);
    if (token.type != RPBIG)
        c = t->child = sySourceElements(t);
    match(RPBIG);
    return t;
}

TreeNode* Parser::sySourceElements(TreeNode* t){
	t = new TreeNode(scanner->getLine(), SourceElements, t);
    TreeNode* c = NULL;
    c = t->child = sySourceElement(t);
	while (token.type != RPBIG && token.type != ENDFILE)
        c = c->sibling = sySourceElement(t);
    return t;
}

TreeNode* Parser::sySourceElement(TreeNode* t){
	t = new TreeNode(scanner->getLine(), SourceElement, t);
    TreeNode* c = NULL;
    if (token.type == FUNCTION)
        c = t->child = syFunctionDeclaration(t);
    else
        c = t->child = syStatement(t);
    return t;
}

TreeNode* Parser::syStatement(TreeNode* t){   //incomplete
	t = new TreeNode(scanner->getLine(), Statement, t);
    TreeNode* c = NULL;
    switch (token.type){
        case VAR:
            c = t->child = syVariableStatement(t);
            break;
		case RETURN:
			c = t->child = syReturnStatement(t);
			break;
		case IF:
			c = t->child = syIfStatement(t);
			break;
        default:
            c = t->child = syExpressionStatement(t);
    }
    return t;
}

TreeNode* Parser::syIfStatement(TreeNode* t){
	t = new TreeNode(scanner->getLine(), IfStatement, t);
	TreeNode* c = NULL;
	match(IF);
	match(LPSMALL);
	c = t->child = syExpression(t);
	match(RPSMALL);
	c = c->sibling = syStatement(t);
	if (token.type == ELSE){
		match(ELSE);
		c->sibling = syStatement(t);
	}
	return t;
}

TreeNode* Parser::syReturnStatement(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ReturnStatement, t);
	TreeNode* c = NULL;
	match(RETURN);
	switch (token.type) {
		case ID:
		case LPSMALL:
		case INTEGER:
		case DECIMAL:
		case HEX:
		case STRING:
		case VBOOLEAN:
		case NIL:
		case NANUMBER:
		case UNDEFINED:
		case INF:
		case FUNCTION:
		case THIS:
			c = t->child = syExpression(t);
			break;
		default:
			break;
	}
	if (token.type == SEMI) match(SEMI);
	return t;
}

TreeNode* Parser::syExpressionStatement(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ExpressionStatement, t);
    TreeNode* c = NULL;
    c = t->child = syExpression(t);
    if (token.type == SEMI) match(SEMI);
    return t;
}

TreeNode* Parser::syExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Expression, t);
    TreeNode* c = NULL;
    c = t->child = syAssignmentExpression(t);
	while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syAssignmentExpression(t);
    }
    return t;
}

TreeNode* Parser::syAssignmentExpression(TreeNode* t){       //AssignmentExpression -> ( LeftHandSideExpression AssignmentOperator AssignmentExpression ) | ( ConditionalExpression )
	t = new TreeNode(scanner->getLine(), AssignmentExpression, t);
    TreeNode* c = NULL;

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
			c = t->child = syConditionalExpression(t);
            break;
		default:{

			reservedLeftExpressions.push_back(syLeftHandSideExpression(t));
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
				c = c->sibling = syAssignmentOperator(t);
				c = c->sibling = syAssignmentExpression(t);
				break;
			default:
				c = t->child = syConditionalExpression(t);
				break;
			}
			return t;
			break;
		}
    }
}

TreeNode* Parser::syLeftHandSideExpression(TreeNode* t){      //LeftHandSideExpression -> MemberExpression [ Arguments ( CallExpressionPart )* ];
	t = new TreeNode(scanner->getLine(), LeftHandSideExpression, t);
    TreeNode* c = NULL;
    c = t->child = syMemberExpression(t);
    if (token.type == LPSMALL){
        c = c->sibling = syArguments(t);
        while (token.type == LPSMALL || token.type == LPMID || token.type == DOT)
            c = c->sibling = syCallExpressionPart(t);
    }
    return t;
}

TreeNode* Parser::syMemberExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), MemberExpression, t);
    TreeNode* c = NULL;

    if (token.type == NEW){                 //incomplete

    }
    else{
        if (token.type == FUNCTION)
            c = t->child = syFunctionExpression(t);
        else
            c = t->child = syPrimaryExpression(t);
        while (token.type == LPMID || token.type == DOT){
            c = c->sibling = syMemberExpressionPart(t);
        }
    }
    return t;
}

TreeNode* Parser::syMemberExpressionPart(TreeNode* t){
	t = new TreeNode(scanner->getLine(), MemberExpressionPart, t);
    TreeNode* c = NULL;

    if (token.type == LPMID){
        match(LPMID);
        c = t->child = syExpression(t);
        match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier(t);
    }
    return t;
}

TreeNode* Parser::syCallExpressionPart(TreeNode* t){
	t = new TreeNode(scanner->getLine(), CallExpressionPart, t);
    TreeNode* c = NULL;
    if (token.type == LPSMALL)
        c = t->child = syArguments(t);
    else if (token.type == LPMID) {
            match(LPMID);
            c = t->child = syExpression(t);
            match(RPMID);
    }
    else {
        match(DOT);
        c = t->child = syIdentifier(t);
    }
    return t;
}

TreeNode* Parser::syArguments(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Arguments, t);
    TreeNode* c = NULL;
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = t->child = syArgumentList(t);
    match(RPSMALL);
    return t;
}

TreeNode* Parser::syArgumentList(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ArgumentList, t);
    TreeNode* c = NULL;
    c = t->child = syAssignmentExpression(t);
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syAssignmentExpression(t);
    }
    return t;
}

TreeNode* Parser::syFunctionExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), FunctionExpression, t);
    TreeNode* c = NULL;
    match(FUNCTION);
    if (token.type == ID) c = t->child = syIdentifier(t);
    else c = t->child = new TreeNode(scanner->getLine(), Identifier, t);
    match(LPSMALL);
    if (token.type != RPSMALL)
        c = t->child = syFormalParameterList(t);
    match(RPSMALL);
    c = c->sibling = syFunctionBody(t);
    return t;
}

TreeNode* Parser::syAssignmentOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), AssignmentOperator, t);
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

TreeNode* Parser::syConditionalExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ConditionalExpression, t);
    TreeNode* c = NULL;
    c = t->child = syLogicalORExpression(t);
    if (token.type == QUES) {
        match(QUES);
        c = c->sibling = syAssignmentExpression(t);
        match(COLON);
        c = c->sibling = syAssignmentExpression(t);
    }
    return t;
}

TreeNode* Parser::syLogicalORExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), LogicalORExpression, t);
    TreeNode* c = NULL;
    c = t->child = syLogicalANDExpression(t);
    while (token.type == OROR){
        c = c->sibling = syLogicalOROperator(t);
        c = c->sibling = syLogicalANDExpression(t);
    }
    return t;
}

TreeNode* Parser::syLogicalOROperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), LogicalOROperator, t);
    TreeNode* c = NULL;
	t->token = token;
    match(OROR);
    return t;
}

TreeNode* Parser::syLogicalANDExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), LogicalANDExpression, t);
    TreeNode* c = NULL;
    c = t->child = syBitwiseORExpression(t);
    while (token.type == ANDAND){
        c = c->sibling = syLogicalANDExpression(t);
        c = c->sibling = syBitwiseORExpression(t);
    }
    return t;
}

TreeNode* Parser::syLogicalANDOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), LogicalANDOperator, t);
    TreeNode* c = NULL;
	t->token = token;
    match(ANDAND);
    return t;
}

TreeNode* Parser::syBitwiseORExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseORExpression, t);
    TreeNode* c = NULL;
    c = t->child = syBitwiseXORExpression(t);
    while (token.type == OR){
        c = c->sibling = syBitwiseOROperator(t);
        c = c->sibling = syBitwiseXORExpression(t);
    }
    return t;
}

TreeNode* Parser::syBitwiseOROperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseOROperator, t);
	t->token = token;
    match(OR);
    return t;
}

TreeNode* Parser::syBitwiseXORExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseXORExpression, t);
    TreeNode* c = NULL;
    c = t->child = syBitwiseANDExpression(t);
    while (token.type == XOR){
        c = c->sibling = syBitwiseXOROperator(t);
        c = c->sibling = syBitwiseANDExpression(t);
    }
    return t;
}

TreeNode* Parser::syBitwiseXOROperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseXOROperator, t);
	t->token = token;
    match(XOR);
    return t;
}

TreeNode* Parser::syBitwiseANDExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseANDExpression, t);
    TreeNode* c = NULL;
    c = t->child = syEqualityExpression(t);
    while (token.type == AND){
        c = c->sibling = syBitwiseANDOperator(t);
        c = c->sibling = syEqualityExpression(t);
    }
    return t;
}

TreeNode* Parser::syBitwiseANDOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), BitwiseANDOperator, t);
	t->token = token;
    match(AND);
    return t;
}


TreeNode* Parser::syEqualityExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), EqualityExpression, t);
    TreeNode* c = NULL;
    c = t->child = syRelationalExpression(t);
    while (token.type == EQUAL || token.type == UNEQUAL){  //incomplete
        c = c->sibling = syEqualityOperator(t);
        c = c->sibling = syRelationalExpression(t);
    }
    return t;
}

TreeNode* Parser::syEqualityOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), EqualityOperator, t);
	t->token = token;
    if (token.type == EQUAL) match(EQUAL);
    else match(UNEQUAL);
    return t;
}

TreeNode* Parser::syRelationalExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), RelationalExpression, t);
    TreeNode* c = NULL;
    c = t->child = syShiftExpression(t);
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case LESS:
            case GREAT:
            case LESSEUQAL:
            case GREATQUAL:
            case INSTANCEOF:
            case IN:
                c = c->sibling = syRelationalOperator(t);
                    c = c->sibling = syShiftExpression(t);
                break;
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syRelationalOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), RelationalOperator, t);
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

TreeNode* Parser::syShiftExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ShiftExpression, t);
    TreeNode* c = NULL;
    c = t->child = syAdditiveExpression(t);
    bool yes = true;;
    while (yes){
        switch (token.type) {           //incomplete
            case LSHIFT:
            case RSHIFT:
                c = c->sibling = syShiftOperator(t);
                c = c->sibling = syAdditiveExpression(t);
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syShiftOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ShiftOperator, t);
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

TreeNode* Parser::syAdditiveExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), AdditiveExpression, t);
    TreeNode* c = NULL;
    c = t->child = syMultiplicativeExpression(t);
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case PLUS:
            case MINUS:
                c = c->sibling = syAdditiveOperator(t);
                c = c->sibling = syMultiplicativeExpression(t);
				break;
            default:
                yes = false;
                break;
        }
    }
    return t;
}

TreeNode* Parser::syAdditiveOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), AdditiveOperator, t);
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

TreeNode* Parser::syMultiplicativeExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), MultiplicativeExpression, t);
    TreeNode* c = NULL;
    c = t->child = syUnaryExpression(t);
    bool yes = true;;
    while (yes){
        switch (token.type) {
            case TIMES:
            case OVER:
            case MOD:
                c = c->sibling = syMultiplicativeOperator(t);
                c = c->sibling = syUnaryExpression(t);
				break;
            default:
                yes = false;
                break;
        }
    }
    return t;
}


TreeNode*  Parser::syMultiplicativeOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), MultiplicativeOperator, t);
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

TreeNode*  Parser::syUnaryExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), UnaryExpression, t);
    TreeNode* c = NULL;
	if (reservedLeftExpressions.empty()){
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
			c = t->child = syUnaryOperator(t);
			c = c->sibling = syUnaryExpression(t);
			break;
		default:
			c = t->child = syPostfixExpression(t);
			break;
		}
	} else
		c = t->child = syPostfixExpression(t);
    return t;
}


TreeNode*  Parser::syUnaryOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), UnaryOperator, t);
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


TreeNode*  Parser::syPostfixExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), PostfixExpression, t);
	TreeNode* c = NULL;
	if (reservedLeftExpressions.empty())
		c = t->child = syLeftHandSideExpression(t);
	else{
		c = t->child = reservedLeftExpressions.back();
		reservedLeftExpressions.pop_back();
	}
    if (token.type == INCREASE || token.type == DECREASE)
        c = c->sibling = syPostfixOperator(t);
    return t;
}

TreeNode* Parser::syPostfixOperator(TreeNode* t){
	t = new TreeNode(scanner->getLine(), PostfixOperator, t);
    t->token = token;
    if (token.type == INCREASE) match(INCREASE);
    else match(DECREASE);
    return t;
}

TreeNode* Parser::syVariableStatement(TreeNode* t){
	t = new TreeNode(scanner->getLine(), VariableStatement, t);
    TreeNode* c = NULL;
    match(VAR);
    c = t->child = syVariableDeclarationList(t);
    if (token.type == SEMI)
        match(SEMI);
    return t;
}

TreeNode* Parser::syVariableDeclarationList(TreeNode* t){
	t = new TreeNode(scanner->getLine(), VariableDeclarationList, t);
    TreeNode* c = NULL;
    c = t->child = syVariableDeclaration(t);
    while (token.type == COMMA){
        match(COMMA);
        c = c->sibling = syVariableDeclaration(t);
    }
    return t;
}

TreeNode* Parser::syVariableDeclaration(TreeNode* t){
	t = new TreeNode(scanner->getLine(), VariableDeclaration, t);
    TreeNode* c = NULL;
    c = t->child = syIdentifier(t);
    if (token.type == ASSIGN)
        c = c->sibling = syInitialiser(t);
    return t;
}

TreeNode* Parser::syInitialiser(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Initialiser, t);
    TreeNode* c = NULL;
    match(ASSIGN);
    c = t->child = syAssignmentExpression(t);
    return t;
}

TreeNode* Parser::syPrimaryExpression(TreeNode* t){
	t = new TreeNode(scanner->getLine(), PrimaryExpression, t);
    TreeNode* c = NULL;
    switch (token.type) {
        case ID:
            c = t->child = syIdentifier(t);
            break;
        case LPSMALL:
            match(LPSMALL);
            c = t->child = syExpression(t);
            match(RPSMALL);
            break;
        case INTEGER:
        case DECIMAL:
        case HEX:
        case STRING:
		case VBOOLEAN:
		case NIL:
		case NANUMBER:
		case UNDEFINED:
		case INF:
            c = t->child = syLiteral(t);
            break;
        case LPMID:
            c = t->child = syArrayLiteral(t);
            break;
        case THIS:
            //match(THIS);          //incomplete
            //break;
        case LPBIG:
            //c = t->child = syObjectLiteral(t);
            //break;
        default:
            throw ParserException();
    }
    return t;
}

TreeNode* Parser::syLiteral(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Literal, t);
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
		case VBOOLEAN:
			match(VBOOLEAN);
			break;
		case NIL:
			match(NIL);
			break;
		case NANUMBER:
			match(NANUMBER);
			break;
		case UNDEFINED:
			match(UNDEFINED);
			break;
		case INF:
			match(INF);
			break;
        default:
            throw ParserException();
    }
    return t;
}

TreeNode* Parser::syArrayLiteral(TreeNode* t){
	t = new TreeNode(scanner->getLine(), ArrayLiteral, t);
    TreeNode* c = NULL;
    match(LPMID);
    while (token.type != RPMID){
        if (token.type == COMMA)
            if (t->child == NULL) c = t->child = syElision(t);
            else c = c->sibling = syElision(t);
        else {
            if (t->child == NULL) c = t->child = syAssignmentExpression(t);
            else c = c->sibling = syAssignmentExpression(t);
            if (token.type != ']') match(COMMA);
        }
    }
    match(RPMID);
    return t;
}

TreeNode* Parser::syElision(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Elision, t);
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

TreeNode* Parser::syProgram(TreeNode* t){
	t = new TreeNode(scanner->getLine(), Program, t);
    TreeNode* c = NULL;
    if (token.type != ENDFILE)
        c = t->child = sySourceElements(t);
    match(ENDFILE);
    return t;
}
