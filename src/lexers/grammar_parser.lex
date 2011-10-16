%{
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "grammar/grammar.h"

/* Automata parser */

static struct transition  transitions[MAX_PRODUCTIONS];
static int                trans_count = 0;
static int                symbols_numbers[MAX_SYMBOLS];
static int                non_terminal_count = 0;
static bool               symbol_is_final[MAX_SYMBOLS] = {0};
static bool               final = false;

/* Grammar parser */

static bool used[255];
static bool terminal[255];

static struct grammar* g;
static bool distinguished;

static char left_part;
static char right_part[3];
static int right;

%}

GRAMMARID [0-9a-zA-Z]*

BEGINGRAMMAR G{GRAMMARID}[:blank:]*=[:blank:]*(

CHAR [a-zA-Z0-9]

%x inGrammar
%x inNonTerminal
%x inTerminal
%x inDistinguished
%x inBeginProduction
%x inEndProduction


ID          [0-9]+

DIGRAPH     digraph[:blank:]*{

FINALDEF    node\[shape=doublecircle][:blank:]Node
NODEDEF     node\[shape=circle][:blank:]Node

TRANS_A     Node
TRANS_B     ->Node
TRANS_C     [:blank:][label=\"

%x automataParser
%x scanNodeSymbol
%x scanTransB
%x scanTransC
%x scanTransD

%%

BEGINGRAMMAR    BEGIN(inGrammar);

<inGrammar>\{   BEGIN(inNonTerminal);

<inNonTerminal>{
    CHAR    {
                add_symbol(g, false, yytext[0]);
                used[(int)yytext[0]] = true;
                terminal[(int)yytext[0]] = false;
            }
    \}          BEGIN(inTerminal);
}

<inTerminal>{
    CHAR    {
                add_symbol(g, true, yytext[0]);
                used[(int)yytext[0]] = true;
                terminal[(int)yytext[0]] = true;
            }
    \}          BEGIN(inDistinguished);
}

<inDistinguished>{
    CHAR    {
                if (!used[(int)yytext[0]] || terminal[(int)yytext[0]]) {
                    // Error
                }
                set_distinguished_symbol(g, yytext[0]);
                distinguished = true;
            }
    ,       { 
                if (!distinguished) {
                    // Error
                }
                BEGIN(inBeginProduction);
            }
}

<inBeginProduction>{
    CHAR    {
                if (!used[(int)yytext[0]] || terminal[(int)yytext[0]]) {
                    // Error
                }
                if (left_part != '\0') {
                    // Error
                }
                left_part = yytext[0];
            }
    ->      {
                if (left_part == '\0') {
                    // Error
                }
                right = right_part[0] = right_part[1] = '\0';
                BEGIN(inEndProduction);
            }
    \}      {
                if (left_part != '\0') {
                    // Error
                }
            }
}
<inEndProduction>{
    CHAR    {
                if (!used[(int)yytext[0]]) {
                    // Error
                }
                right_part[right++] = yytext[0];
            }
    ,       {
                if (right == 0) {
                    // Error
                }
                add_production(g, left_part, right_part);
                left_part = '\0';
                BEGIN(inBeginProduction);
            }
    \|      {
                add_production(g, left_part, right_part);
                right = right_part[0] = right_part[1] = '\0';
            }

    \}      {
                if (right == 0) {
                    // Error
                }
                add_production(g, left_part, right_part);
                BEGIN(inGrammar);
            }
}

DIGRAPH             {
                        BEGIN(automataParser);
                    }

<automataParser>{
    FINALDEF        {
                        final = true;
                        BEGIN(scanNodeSymbol);
                    }

    NODEDEF             BEGIN(scanNodeSymbol);


    TRANS_A             BEGIN(scanTransB);

}

<scanNodeSymbol>ID  {
                        symbol_is_final[non_terminal_count] = final;
                        final = false;
                        symbols_numbers[non_terminal_count++] = atoi(yytext);
                        BEGIN(automataParser);
                    }

<scanTransB>ID          transitions[trans_count].from = atoi(yytext);

<scanTransB>TRANS_B     BEGIN(scanTransC);

<scanTransC>ID          transitions[trans_count].to = atoi(yytext);

<scanTransC>TRANS_C     BEGIN(scanTransD);

<scanTransD>\\\\    {
                        transitions[trans_count++].symbol = '\\';
                    }

<scanTransD>CHAR    {
                        transitions[trans_count++].symbol = yytext[0];
                    }

<scanTransD>/       {
                        transitions[trans_count].from =
                            transitions[trans_count-1].from;
                        transitions[trans_count].to =
                            transitions[trans_count-1].to;
                    }

<scanTransD>\"      {
                        BEGIN(automataParser);
                    }


.           // Pass

%%

int yywrap(void) {
    return 1;
}

struct grammar* parse_grammar_file(char* filename) {

    g = create_grammar();

    yyin = fopen(filename, "r");
    yylex();

    return g;
}

struct grammar* parse_automata_file(char* filename, struct automata **a) {

    yyin = fopen(filename, "r");
    yylex();

    g = create_grammar();

    *a = (struct automata*)malloc(sizeof(struct automata));
    memset(*a, 0, sizeof(struct automata));
    struct automata *b = *a;

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
        b->states[b->number_states++] = preffered;

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

