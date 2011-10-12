typedef enum {false = 0, true} bool;

struct grammar {
	int                number_symbols;
	char**             symbol_names;
	int                number_productions;
	struct production* productions;
	int                distinguished_symbol_index;
};

struct symbol_array {
	int            size;
	struct symbol* symbols;
};

struct production {
	struct symbol_array left_part;
	struct symbol_array right_part;
};

struct symbol {
	bool terminal;
	int  symbol_name_index;
};

