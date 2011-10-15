%{
#include <stdlib.h>
#include <ctype.h>
#include "grammar/grammar.h"
%}
%%

%%

static int yywrap(void) {
    return 1;
}

struct grammar* parse_automata_file(char* filename) {

    struct grammar* g = create_grammar();

    yyin = fopen(filename, "r");
    yylex();

    return g;
}

