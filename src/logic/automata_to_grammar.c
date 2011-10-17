#include "logic/grammar.h"

#include <stdlib.h>
#include <string.h>

struct grammar* automata_to_grammar(struct automata* b) {

    struct grammar* g = create_grammar();

    bool t_used[255];
	bool nt_used[255];
    memset(t_used, 0, 255);
	memset(nt_used, 0, 255);

    char number_to_symbol[255];
    for (int i = 0; i < b->number_transitions; i++) {
        add_symbol(g, true, b->transitions[i].symbol);
        nt_used[(int)b->transitions[i].symbol] = true;
    }


    int preffered = 'A';
    for (int i = 0; i < b->number_states; i++) {
        
        while(preffered < 255 && t_used[preffered++]);
		preffered--;
        add_symbol(g, false, preffered);
        number_to_symbol[i] = preffered;
        t_used[preffered] = true;

        if (b->final_state[i]) {
            char lambda_production[2] = { '\\', '\0' };
            add_production(g, (char)preffered, lambda_production);
        }
    }

    set_distinguished_symbol(g, (char)number_to_symbol[0]);

    for (int i = 0; i < b->number_transitions; i++) {
        
        char prod[3] = { 
            b->transitions[i].symbol,
            number_to_symbol[b->transitions[i].to],
			0
        };
        add_production(g, number_to_symbol[b->transitions[i].from], prod);
    }

    return g;
}

