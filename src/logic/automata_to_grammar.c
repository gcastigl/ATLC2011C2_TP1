#include "logic/grammar.h"

#include <stdlib.h>
#include <string.h>

struct grammar* automata_to_grammar(struct automata* b) {

    struct grammar* g = create_grammar();

    bool used[255];
    memset(used, 0, 255);

    char number_to_symbol[255];

    for (int i = 0; i < b->number_transitions; i++) {
        add_symbol(g, true, b->transitions[i].symbol);
        used[(int)b->transitions[i].symbol] = true;
    }


    int preffered = 'A';
    for (int i = 0; i < b->number_states; i++) {
        
        while(preffered < 255 && used[preffered++]);
        add_symbol(g, false, preffered);
        number_to_symbol[i] = preffered;
        used[preffered] = true;

        if (b->final_state[i]) {
            char lambda_production[2] = { '\\', '\0' };
            add_production(g, (char)preffered, lambda_production);
        }
    }

    set_distinguished_symbol(g, (char)number_to_symbol[0]);

    for (int i = 0; i < b->number_transitions; i++) {
        
        char prod[2] = { 
            b->transitions[i].symbol,
            number_to_symbol[b->transitions[i].to]
        };

        add_production(g, number_to_symbol[b->transitions[i].from], prod);
    }

    return g;
}

