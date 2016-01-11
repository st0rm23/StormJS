#include "scanner.h"
#include "parser.h"
#include "cstdlib"

int main(){
    FILE* file = fopen("function.js", "r");
	Parser* parser = new Parser(file);
	try{
		TreeNode* node = parser->syStatement();
		fprintf(stderr, "\nsucceeding in parsing !\n");
	}
	catch (ParserException &e){
		parser->printError();
	}
	delete parser;
	return 0;
}
