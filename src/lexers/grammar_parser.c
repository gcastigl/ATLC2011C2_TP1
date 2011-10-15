%{
#include <stdlib.h>
#include <ctype.h>
%}
%%

%%
static int yywrap(void) {
	return 1;
}

struct grammar* parse_grammar_file(char* filename) {

    struct grammar* g = create_grammar();

    yyin = fopen(filename, "r");
    yylex();

    return g;
}

