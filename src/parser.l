%{
#include <stdlib.h>
#include <ctype.h>
%}
%%

%%
int yywrap(void) {
	return 1;
}

int main(void) {
	yyin = fopen("../resources/archivos\ gr/gr1.gr", "r");
	yylex();
	return 0;
}

