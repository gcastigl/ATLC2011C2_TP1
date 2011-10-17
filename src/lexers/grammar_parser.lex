%{
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "logic/grammar.h"

/* Automata parser */

static struct automata    *b;
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

BEGINGRAMMAR G{GRAMMARID}

CHAR [a-zA-Z0-9]

%x inGrammar
%x inNonTerminal
%x inTerminal
%x inDistinguished
%x inBeginProduction
%x inEndProduction


ID          [0-9]+

DIGRAPH     digraph

FINALDEF    node\[shape=doublecircle][ ]Node
NODEDEF     node\[shape=circle][ ]Node

TRANS_A     Node
TRANS_B     ->Node
TRANS_C     \[label\=\"

%x automataParser
%x scanNodeSymbol
%x scanTransB
%x scanTransC
%x scanTransD

%%

{BEGINGRAMMAR}  {
                BEGIN(inGrammar);
}

<inGrammar>\{   {
                BEGIN(inNonTerminal);
}

<inNonTerminal>{
    {CHAR}  {
                add_symbol(g, false, yytext[0]);
                used[(int)yytext[0]] = true;
                terminal[(int)yytext[0]] = false;
            }
    \}      {
                BEGIN(inTerminal);
            }
}

<inTerminal>{
    {CHAR}  {
                add_symbol(g, true, yytext[0]);
                used[(int)yytext[0]] = true;
                terminal[(int)yytext[0]] = true;
            }
    \},     {
                BEGIN(inDistinguished);
            }
}

<inDistinguished>{
    {CHAR}  {
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
    {CHAR}  {
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
    {CHAR}  {
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

{DIGRAPH}   {
                BEGIN(automataParser);
}

<automataParser>{
    {FINALDEF}  {
                final = true;
                BEGIN(scanNodeSymbol);
    }

    {NODEDEF}   {
                final = false;
                BEGIN(scanNodeSymbol);
    }

    {TRANS_A}   {
                BEGIN(scanTransB);
    }
}

<scanNodeSymbol>{
    {ID} {
                b->final_state[b->number_states] = final;
                b->states[b->number_states++] = yytext[0];
                BEGIN(automataParser);
    }
}

<scanTransB>{
    {ID}        b->transitions[b->number_states].from = yytext[0];

    {TRANS_B}   BEGIN(scanTransC);
}

<scanTransC>{
    {ID}    {
                b->transitions[b->number_transitions].to = yytext[0];
    }

    {TRANS_C} {
                BEGIN(scanTransD);
    }
}

<scanTransD>{

    \\\\     {
                b->transitions[b->number_transitions++].symbol = '\\';

                if (!used['\\']) {
                    used['\\'] = true;
                    b->chars[b->number_chars++] = '\\';
                }
    }

    >{CHAR}  {
                b->transitions[b->number_transitions++].symbol =
                    yytext[0];

                if (!used[(int)yytext[0]]) {
                    used[(int)yytext[0]] = true;
                    b->chars[b->number_chars++] = yytext[0];
                }
    }

    >\/      {
                b->transitions[b->number_transitions].from =
                    b->transitions[b->number_transitions-1].from;
                b->transitions[b->number_transitions].to =
                    b->transitions[b->number_transitions-1].to;
    }

    \"       {
                BEGIN(automataParser);
    }
}

\n                      // Pass

<inGrammar>\n           // Pass
<inNonTerminal>\n       // Pass
<inTerminal>\n          // Pass
<inDistinguished>\n     // Pass
<inBeginProduction>\n   // Pass
<inEndProduction>\n     // Pass

<automataParser>\n      // Pass
<scanNodeSymbol>\n      // Pass
<scanTransB>\n          // Pass
<scanTransC>\n          // Pass
<scanTransD>\n          // Pass

.                       // Pass

<inGrammar>.            // Pass
<inNonTerminal>.        // Pass
<inTerminal>.           // Pass
<inDistinguished>.      // Pass
<inBeginProduction>.    // Pass
<inEndProduction>.      // Pass

<automataParser>.       // Pass
<scanNodeSymbol>.       // Pass
<scanTransB>.           // Pass
<scanTransC>.           // Pass
<scanTransD>.           // Pass

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

struct automata* parse_automata_file(char* filename) {

    b = (struct automata*)malloc(sizeof(struct automata));
    memset(b, 0, sizeof(struct automata));

    yyin = fopen(filename, "r");
    yylex();

    return b;
}

