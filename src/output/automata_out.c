#include <string.h>

#include "output/output.h"

static int char_map[255];
static char integer_map[255];

void automata_output(FILE* outfile, struct grammar* grammar) {

    memset(char_map, 0, sizeof(char_map));

    fprintf(outfile, "digraph{\n\trankdir=\"LR\";\n\t//Nodos\n\t");

    int nonterminal_count = 0;

    integer_map[(int)grammar->distinguished_symbol] = nonterminal_count;
    char_map[nonterminal_count++] = grammar->distinguished_symbol;

    for (int i = 0; i < grammar->number_symbols; i++) {

        if (grammar->symbols[i].terminal == false &&
            grammar->symbols[i].representation != grammar->distinguished_symbol
        ) {
            integer_map[(int)grammar->symbols[i].representation] = nonterminal_count;
            char_map[nonterminal_count++] = grammar->symbols[i].representation;
        }
    }

    for (int i = 0; i < grammar->number_symbols; i++) {

        if (grammar->symbols[i].terminal == false) {
            int index = integer_map[(int)grammar->symbols[i].representation];
            fprintf(outfile, "node[shape=circle] Node%d [label=\"%d\"];\n\t",
                index, index
            );
        }
    }

    fprintf(outfile, "node[shape=doublecircle] Node%d [label=\"%d\"];\n\t",
        nonterminal_count, nonterminal_count
    );

    fprintf(outfile, "//Transiciones\n\t");

    for (int i = 0; i < grammar->number_productions; i++) {

        struct symbol* left_part = &(grammar->productions[i].left_part);
        struct symbol* right_part1 = &(grammar->productions[i].right_part[0]);
        struct symbol* right_part2 = &(grammar->productions[i].right_part[1]);

        int from_number, to_number;
        char transition_char[3] = {0, 0, 0};

        from_number = integer_map[(int)left_part->representation];
        // A -> A  Production
        if (right_part1->terminal == false) {
            to_number = integer_map[(int)right_part1->representation];
            transition_char[0] = '\\';
            transition_char[1] = '\\';
        }
        // A -> a  Production
        else if (right_part2->representation == '\0') {
            to_number = nonterminal_count;
            transition_char[0] = right_part1->representation;
            // A -> \\ Production
            if (right_part1->representation == '\\') {
                to_number = nonterminal_count;
                transition_char[0] = '\\';
                transition_char[1] = '\\';
            }
        }
        // A -> aA Production
        else if (right_part1->terminal == true &&
                 right_part2->terminal == false
        ){
            to_number = integer_map[(int)right_part2->representation];
            transition_char[0] = right_part1->representation;
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

    fprintf(outfile, "\n}\n");
}

