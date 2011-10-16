#include "grammar/grammar.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

struct grammar* create_grammar() {
    struct grammar* grammar = malloc(sizeof(struct grammar));
    
    if (grammar == NULL) {
        return NULL;
    }

    grammar->number_symbols = 0;
    grammar->number_productions = 0;
    grammar->alignment = UNDEFINED_ALIGNMENT;

    return grammar;
}

static bool symbol_exists(struct grammar* grammar, char symbol) {

    for (int i = 0; i < grammar->number_symbols; i++) {
        if (grammar->symbols[i].representation == symbol) {
            return true;
        }
    }
    return false;
}

static bool symbol_is_terminal(struct grammar* grammar, char symbol) {
    int i;
    for (i = 0; i < grammar->number_symbols; i++) {
        if (grammar->symbols[i].representation == symbol &&
            grammar->symbols[i].terminal)
        {
            return true;
        }
    }
    return false;
}

static bool symbol_is_not_terminal(struct grammar* grammar, char symbol) {
    if (symbol_exists(grammar, symbol) &&
        !symbol_is_terminal(grammar, symbol))
    {
        return true;
    }
    return false;
}

int destroy_grammar(struct grammar* grammar) {
    free(grammar);
    return 0;
}

int add_symbol(struct grammar* grammar, bool terminal, char symbol) {
    if (symbol_exists(grammar, symbol)) {
        return -1;
    }
    grammar->symbols[grammar->number_symbols].terminal = terminal;
    grammar->symbols[grammar->number_symbols].representation = symbol;
    (grammar->number_symbols)++;
    return 0;
}

int set_distinguished_symbol(struct grammar* grammar, char symbol) {
    if(symbol_is_not_terminal(grammar, symbol)) {
        grammar->distinguished_symbol = symbol;
        return 0;
    }
    return -1;
}   

int add_production(struct grammar* grammar, char left_part, char* right_part) {
    
    int len = strlen(right_part);    
    
    if (symbol_is_terminal(grammar, left_part)) {
        return -1;
    }
    if (!symbol_exists(grammar, right_part[0])) {
        return -1;
    }
    if (len == 2) {
        if (grammar->alignment == UNDEFINED_ALIGNMENT) {
            if (!symbol_exists(grammar, right_part[1])) {
                return -1;
            }
            if (symbol_is_terminal(grammar, right_part[0]) &&
                symbol_is_not_terminal(grammar, right_part[1]))
            {
                grammar->alignment =  RIGHT_ALIGNED;
            } else if (symbol_is_not_terminal(grammar, right_part[0]) &&
                       symbol_is_terminal(grammar, right_part[1]))
            {
                grammar->alignment = LEFT_ALIGNED;
            } else {
                return -1;
            }
        } else if (grammar->alignment == RIGHT_ALIGNED) {
            if (!(symbol_is_terminal(grammar, right_part[0]) &&
                  symbol_is_not_terminal(grammar, right_part[1]))
            ){
                return -1;
            }
        } else {
            if (!(symbol_is_not_terminal(grammar, right_part[0]) &&
                  symbol_is_terminal(grammar, right_part[1]))
            ){
                return -1;
            }
        }
    }

    struct symbol *left_symbol = grammar->
        productions[grammar->number_productions].left_part;
    left_symbol->representation = left_part;
    left_symbol->terminal = symbol_is_terminal(grammar, left_part);

    struct symbol *right_symbol1 = grammar->
        productions[grammar->number_productions].right_part[0];

    right_symbol1->representation = right_part[0];
    right_symbol1->terminal = symbol_is_terminal(grammar, right_part[0]);

    struct symbol *right_symbol2 = grammar->
        productions[grammar->number_productions].right_part[1];

    if (len == 2) {
        right_symbol2->representation = right_part[1];
        right_symbol2->terminal = symbol_is_terminal(grammar, right_part[1]);
    } else {
        right_symbol2->representation = '\0';
    }

    (grammar->number_productions)++;

    return 0;
}

static void production_bfs(struct grammar* grammar,
    (void*)f(struct grammar*, void*, void*), void*)
{
}

static void check_reachable(struct grammar* grammar, 
    struct production* production, bool* reach
){
    if (symbol_is_not_terminal(grammar,
            production->right_part[0].representation)) 
    {
        reach[production->right_part[0].representation] = true;
    }
    if (symbol_is_not_terminal(grammar,
            production->right_part[1].representation)) 
    {
        reach[production->right_part[1].representation] = true;
    }
}

struct grammar* take_out_unreachable(struct grammar* source) {
    
    struct grammar* new = create_grammar();    

    bool reachable_symbol[MAX_SYMBOLS];
    memset(reachable_symbol, 0, MAX_SYMBOLS);

    production_bfs(source, check_reachable, reachable_symbol);
}

struct grammar* to_right_normal_form(struct grammar* source) {

    struct grammar* new = create_grammar();    

    for (int i = 0; i < source->number_symbols; i++) {
        add_symbol(new, source->symbols[i].terminal,
            source->symbols[i].representation
        );
    }

    // This will be our 'M' symbol. We choose something not printable.
    add_symbol(new, false, '\t');

    set_distinguished_symbol(new, '\t');

    for (int i = 0; i < source->number_productions; i++) {

        char right_part[3] = {0, 0, 0};

        right_part[0] = source->productions[i].right_part[0].representation;
        right_part[1] = source->productions[i].right_part[1].representation;

        add_production(new, source->productions[i].left_part.representation,
            right_part
        );
    }

    return new;
}

