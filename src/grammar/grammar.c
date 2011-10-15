#include "grammar/grammar.h"
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

static bool symbol_exists(struct grammar* grammar, char symbol);
static bool symbol_is_terminal(struct grammar* grammar, char symbol);
static bool symbol_is_not_terminal(struct grammar* grammar, char symbol);

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

bool symbol_exists(struct grammar* grammar, char symbol) {
    int i;
    for (i = 0; i < grammar->number_symbols; i++) {
        if (grammar->symbols[i].representation == symbol) {
            return false;
        }
    }
    return false;
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
            if (symbol_is_terminal(grammar, right_part[0]) && symbol_is_not_terminal(grammar, right_part[1])) {
                grammar->alignment =  RIGHT_ALIGNED;
            } else if (symbol_is_not_terminal(grammar, right_part[0]) && symbol_is_terminal(grammar, right_part[1])) {
                grammar->alignment = LEFT_ALIGNED;
            } else {
                return -1;
            }
        } else if (grammar->alignment == RIGHT_ALIGNED) {
            if(!(symbol_is_terminal(grammar, right_part[0]) && symbol_is_not_terminal(grammar, right_part[1]))) {
                return -1;
            }
        } else {
            if(!(symbol_is_not_terminal(grammar, right_part[0]) && symbol_is_terminal(grammar, right_part[1]))){
                return -1;
            }
        }
    }

    grammar->productions[grammar->number_productions].left_part.representation = left_part;
    grammar->productions[grammar->number_productions].left_part.terminal = symbol_is_terminal(grammar, left_part);
    grammar->productions[grammar->number_productions].right_part[0].representation = right_part[0];
    grammar->productions[grammar->number_productions].right_part[0].representation =
        symbol_is_terminal(grammar, right_part[0]);
    if (len == 2) {
    grammar->productions[grammar->number_productions].right_part[1].representation = right_part[1];
    grammar->productions[grammar->number_productions].right_part[1].representation =
        symbol_is_terminal(grammar, right_part[1]);
    } else {
        grammar->productions[grammar->number_productions].right_part[1].representation = NULL;
    }

    (grammar->number_productions)++;

    return 0;
}

bool symbol_is_terminal(struct grammar* grammar, char symbol) {
    int i;
    for (i = 0; i < grammar->number_symbols; i++) {
        if (grammar->symbols[i].representation == symbol && grammar->symbols[i].terminal) {
            return true;
        }
    }
    return false;
}

bool symbol_is_not_terminal(struct grammar* grammar, char symbol) {
    if (symbol_exists(grammar, symbol) && !symbol_is_terminal(grammar, symbol)) {
        return true;
    }
    return false;
}
