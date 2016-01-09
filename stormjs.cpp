#include "scanner.h"
#include "cstdlib"

int main(){
    FILE* file = fopen("function.js", "r");
    Scanner scanner(file);
	while (1){
		Token token = scanner.getToken();
		if (token.type == ERROR){
			fprintf(stderr, "\nline %d error: in char %c\n", scanner.getLine(), scanner.getNowChar());
			break;
		}
		if (token.type == ENDFILE){
			fprintf(stderr, "\nsuceed in scanning\n");
			break;
		}
	};
	return 0;
}
