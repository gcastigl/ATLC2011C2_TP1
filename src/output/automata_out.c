#include "output/output.h"

static int char_map[255];

void automata_output(FILE* outfile, struct grammar* grammar) {

    fprintf(outfile, "digraph{\n\trankdir=\"LR\";\n\t//Nodos\n\t");

    for (int i = 0; i < grammar->number_symbols; i++) {

        char_map[grammar->symbols[i].left_part.representation] = i;
    }

    char previous_map = char_map[grammar->distinguished_symbol];
    char_map[grammar->distinguished_symbol] = 0;
    char_map[grammar->symbols[0].left_part.representation] = previous_map;

    for (int i = 0; i < grammar->number_symbols; i++) {

        int index = char_map[grammar->symbols[i].left_part.representation];

        fprintf(outfile, "node[shape=circle] Node%d [label=\"%d\"];\n\t",
            index, index
        );
    }

    fprintf(outfile, "node[shape=doublecircle] Node%d [label=\"%d\"];\n\t",
        grammar->number_symbols, grammar->number_symbols
    );

    fprintf("//Transiciones\n\t");

    for (int i = 0; i < grammar->number_productions; i++) {

        struct symbol* left_part = grammar->productions[i].left_part;
        struct symbol* right_part1 = grammar->productions[i].right_part[0];
        struct symbol* right_part2 = grammar->productions[i].right_part[1];

        int from_number, to_number;
        char transition_char[3] = {0, 0, 0};

        from_number = char_map[left_part.representation];

        // A -> A  Production
        if (right_part1.terminal == false) {
            to_number = char_map[right_part1.representation];
            transition_char[0] = '\\';
            transition_char[1] = '\\';
        }
        // A -> a  Production
        else if (right_part2.representation == '\0') {
            to_number = grammar->number_symbols;
            transition_char[0] = right_part1.representation;
        }
        // A -> aA Production
        else if (right_part1.terminal == true &&
                 right_part2.terminal == false
        ){
            to_number = char_map[right_part2.representation];
            transition_char[0] = right_part1.representation;
        }
        // A -> \\ Production
        else if (right_part1.representation == '\\') {
            to_number = grammar->number_symbols;
            transition_char[0] = '\\';
            transition_char[1] = '\\';
        }
        else {
            fprintf(stderr, "Error: invalid grammar. "
                "Unrecognized production\n"
            );
            return;
        }

        fprintf(outfile, "Node%d->Node%d [label=\"%s\"];\n\t",
            from_number, to_number, transition_char
        );
    }

    fprintf("}\n");
}

