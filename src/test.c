#include <stdlib.h>
#include <stdio.h>

#include "grammar/grammar.h"
#include "lexers/lexers.h"
#include "output/output.h"

int test(int argc, char** argv) {

    struct grammar* grammar = create_grammar();
    add_non_terminal(grammar, 'A');
    add_non_terminal(grammar, 'B');
    add_non_terminal(grammar, 'C');
    add_terminal(grammar, 'a');
    add_terminal(grammar, 'b');
    add_terminal(grammar, 'c');
    set_distinguished_symbol(grammar, 'A');
    add_production(grammar, 'A', "Ba");
    add_production(grammar, 'B', "Ac");
    add_production(grammar, 'A', "Cc");
    add_production(grammar, 'B', "b"); 

    FILE* stream = fopen( "./TEST", "w");
    struct grammar* right = as_right_normal_form(grammar);
    automata_output(stream, right);
    fclose(stream);

    destroy_grammar(right);
    destroy_grammar(grammar);

    return 0;
}

