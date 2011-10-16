#ifndef __LEXERS__
#define __LEXERS__

#include "logic/grammar.h"

struct automata* parse_automata_file(char*);
struct grammar* parse_grammar_file(char*);

#endif

