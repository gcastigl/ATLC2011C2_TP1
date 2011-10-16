%{
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "grammar/grammar.h"

struct trans {
    int  from;
    int  to;
    char symbol;
};

static struct trans  transitions[MAX_PRODUCTIONS];
static int           trans_count = 0;
static int           symbols_numbers[MAX_SYMBOLS];
static int           non_terminal_count = 0;
static bool          symbol_is_final[MAX_SYMBOLS] = {0};

static bool          final = false;
%}

ID          [0-9]+
CHAR        .

FINALDEF    node\[shape=doublecircle][:blank:]Node
NODEDEF     node\[shape=circle][:blank:]Node

TRANS_A     Node
TRANS_B     ->Node
TRANS_C     [:blank:][label=\"

%x scanNodeSymbol
%x scanTransB
%x scanTransC
%x scanTransD

%%

FINALDEF            {
                        final = true;
                        BEGIN(scanNodeSymbol);
                    }

NODEDEF                 BEGIN(scanNodeSymbol);

<scanNodeSymbol>ID  {
                        symbol_is_final[non_terminal_count] = final;
                        final = false;
                        symbols_numbers[non_terminal_count++] = atoi(yytext);
                        BEGIN(0);
                    }

TRANS_A                 BEGIN(scanTransB);

<scanTransB>ID          transitions[trans_count].from = atoi(yytext);

<scanTransB>TRANS_B     BEGIN(scanTransC);

<scanTransC>ID          transitions[trans_count].to = atoi(yytext);

<scanTransC>TRANS_C     BEGIN(scanTransD);

<scanTransD>CHAR    {
                        transitions[trans_count++].symbol = yytext[0];
                        BEGIN(0);
                    }

[:blank:]           /* Skip */ 

CHAR                /* Skip */

%%

int yywrap(void) {
    return 1;
}

struct grammar* parse_automata_file(char* filename) {

    yyin = fopen(filename, "r");
    yylex();

    struct grammar* g = create_grammar();

    bool used[255];
    memset(used, 0, 255);

    for (int i = 0; i < trans_count; i++) {
        add_symbol(g, true, transitions[i].symbol);
        used[(int)transitions[i].symbol] = true;
    }

    int preffered = 'A';
    for (int i = 0; i < non_terminal_count; i++) {
        
        while(preffered < 255 && used[preffered++]);

        add_symbol(g, false, preffered);
        symbols_numbers[i] = preffered;
        used[preffered] = true;

        if (symbol_is_final[i]) {
            char lambda_production[2] = { '\\', '\0' };
            add_production(g, (char)preffered, lambda_production);
        }
    }

    set_distinguished_symbol(g, (char)symbols_numbers[0]);

    for (int i = 0; i < trans_count; i++) {
        
        char prod[2] = { 
            transitions[i].symbol,
            symbols_numbers[transitions[i].to]
        };

        add_production(g, (char)symbols_numbers[transitions[i].from], prod);
    }

    return g;
}

