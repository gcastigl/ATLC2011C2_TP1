#include "output/output.h"

void output_symbols(bool terminal_required, FILE* outfile,
                    struct grammar* grammar) {

    bool first = true;

    for (int i = 0; i < grammar->number_symbols; i++) {

        if (grammar->symbols[i].terminal == terminal_required) {

            if (!first) {
                fprintf(outfile, ", ");
            }
            first = false;

            fprintf(outfile, "%c", grammar->symbols[i].representation);
        }
    }
}

void grammar_output(FILE* outfile, struct grammar* grammar) {

    fprintf(outfile, "G = (\n\t{");

    output_symbols(false, outfile, grammar);

    fprintf(outfile, "},\n\t{");

    output_symbols(true, outfile, grammar);

    fprintf(outfile, "},\n\t%c,\n\t{", grammar->distinguished_symbol);

    bool first_symbol = true;

    for (int i = 0; i < grammar->number_symbols; i++) {

        if (!grammar->symbols[i].terminal) {

            bool first_production = true;

            for (int j = 0; j < grammar->number_productions; j++) {

                if (grammar->productions[j].left_part.representation == 
                    grammar->symbols[i].representation)
                {

                    if (!first_production) {
                        fprintf(outfile, " | ");
                    } else {
                        
                        if (!first_symbol) {
                            fprintf(outfile, ", ");
                        }
                        fprintf(outfile, "%c -> ",
                            grammar->symbols[i].representation
                        );
                        first_production = false;
                    }
                    first_symbol = false;

                    fprintf(outfile, "%c",
                        grammar->productions[j].right_part[0].representation
                    );

                    if (grammar->productions[j].right_part[1].representation
                        != '\0')
                    {
                        fprintf(outfile, "%c",
                            grammar->productions[j].right_part[1].representation
                        );
                    }
                }
            }
        }
    }
    fprintf(outfile, "\n}\n)\n");
}

