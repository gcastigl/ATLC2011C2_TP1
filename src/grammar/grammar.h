#ifndef __GRAMMAR__
#define __GRAMMAR__

#define MAX_SYMBOLS 1000
#define MAX_PRODUCTIONS 1000
#define add_terminal(x,y) add_symbol((x), 1, (y))
#define add_non_terminal(x,y) add_symbol((x), 0, (y))

#include <stdbool.h>

enum grammar_state { UNDEFINED_ALIGNMENT, RIGHT_ALIGNED, LEFT_ALIGNED };

struct grammar {
    int                                  number_symbols;
    struct symbol[MAX_SYMBOLS]           symbols;
    int                                  number_productions;
	struct production[MAX_PRODUCTIONS]   productions;
	char                                 distinguished_symbol;
    enum grammar_state                   alignment;
};


struct production {
    struct symbol left_part;
    struct symbol[2] right_part;
};

struct symbol {
    bool terminal;
    char representation;
};

struct grammar* create_grammar();
int destroy_grammar(struct grammar* grammar);
int add_symbol(struct grammar* grammar, bool terminal, char symbol_name);
int add_production(struct grammar* grammar, char left_part, char* right_part);
int set_distinguished_symbol(struct grammar* grammar, char symbol);

#endif

