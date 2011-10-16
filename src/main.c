#include <stdlib.h>
#include <stdio.h>

#include "grammar/grammar.h"
#include "lexers/lexers.h"
#include "output/output.h"

int main(int argc, char** argv) {

    struct grammar* grammar = create_grammar();
    add_non_terminal(grammar, 'A');
    add_non_terminal(grammar, 'B');
    add_non_terminal(grammar, 'C');
    add_terminal(grammar, 'a');
    add_terminal(grammar, 'b');
    add_terminal(grammar, 'c');
    set_distinguished_symbol(grammar, 'A');
    add_production(grammar, 'A', "aB");
    add_production(grammar, 'B', "aA");
    add_production(grammar, 'A', "c");
    add_production(grammar, 'B', "b"); 

    FILE* stream = fopen( "./TEST", "w");
    automata_output(stream, grammar);
    fclose(stream);

    destroy_grammar(grammar);

    return 0;
}

