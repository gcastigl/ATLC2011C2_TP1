struct grammar* automata_to_grammar(struct automata* a) {

    g = create_grammar();

    bool used[255];
    memset(used, 0, 255);

    char number_to_symbol[255];

    for (int i = 0; i < a->number_transitions; i++) {
        add_symbol(g, true, a->transitions[i].symbol);
        used[(int)a->transitions[i].symbol] = true;
    }

    int preffered = 'A';
    for (int i = 0; i < b->number_states; i++) {
        
        while(preffered < 255 && used[preffered++]);

        add_symbol(g, false, preffered);

        b->final_state[b->number_states] = b->final_state[i];
        b->states[b->number_states++] = preffered;

        number_to_symbol[i] = preffered;
        used[preffered] = true;

        if (symbol_is_final[i]) {
            char lambda_production[2] = { '\\', '\0' };
            add_production(g, (char)preffered, lambda_production);
        }
    }

    set_distinguished_symbol(g, (char)symbols_numbers[0]);

    for (int i = 0; i < b->number_transitions; i++) {
        
        char prod[2] = { 
            b->transitions[i].symbol,
            number_to_symbol[b->transitions[i].to]
        };

        add_production(g, number_to_symbol[transitions[i].from], prod);
    }

    return g;
}

