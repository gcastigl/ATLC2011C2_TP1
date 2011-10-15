#ifndef __GRAMMAR__
#define __GRAMMAR__

#define MAX_SYMBOLS 1000

typedef enum {false = 0, true} bool;

struct grammar {
	int                number_symbols;
	char**             symbol_names;
	int                number_productions;
	struct production* productions;
	int                distinguished_symbol_index;
    bool               right_aligned;
};


struct production {
	struct symbol left_part;
	struct symbol[2] right_part;
};

struct symbol {
	bool terminal;
	int  symbol_name_index;
};

struct symbol_table {
    int count;
    struct symbol_reference[MAX_SYMBOLS];
}

struct symbol_reference {
    int symbol_name_index;
    char* symbol_name;
}

#endif
