#include "scanner.h"
#include "parser.h"
#include "cstdlib"
#include "global.h"
#include "variable.h"
#include "expression.h"
#include <iostream>

using namespace std;

int main(){
/*	Token token;
	token.type = TokenType::DECIMAL;
	token.value = "123";
	Variable* var1 = NULL, *var2 = NULL;
	var1 = Variable::variableFromToken(&token);

	token.type = TokenType::DECIMAL;
	token.value = "456";
	var2 = Variable::variableFromToken(&token); 


	printf("%s\n", Expression::opMultiplicative(var1, TokenType::TIMES, var2)->toString().c_str());*/
    FILE* file = fopen("expression.js", "r");
	Parser* parser = new Parser(file);
	try{
		TreeNode* node = parser->syProgram();
		fprintf(stderr, "\nsucceeding in parsing !\n");
	}
	catch (ParserException e){
		parser->printError();
	}
	delete parser;
	return 0;
}
