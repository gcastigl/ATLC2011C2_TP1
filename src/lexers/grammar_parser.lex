%{
#include <stdlib.h>
#include <ctype.h>
#include "grammar/grammar.h"

static bool used[255];
static bool terminal[255];

static char curr;
static struct grammar* g;
static bool distinguished;

static char left_part;
static char right_part[3];
static int right;

%}

GRAMMARID [0-9a-zA-Z]*

BEGINGRAMMAR G{GRAMMARID}[:blank:]*=[:blank:]*(

CHAR [a-zA-Z]

%x inGrammar
%x inNonTerminal
%x inTerminal
%x inDistinguished
%x inBeginProduction
%x inEndProduction

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
                right = right_part[0] = right_part[1] = '\0';
            }

    \}      {
                if (right == 0) {
                    // Error
                }
                add_production(g, left_part, right_part);
                BEGIN(0);
            }
}

.           // Pass

%%

struct grammar* parse_grammar_file(char* filename) {

    g = create_grammar();

    yyin = fopen(filename, "r");
    yylex();

    return g;
}

