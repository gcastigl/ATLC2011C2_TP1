#ifndef __OUTPUT__
#define __OUTPUT__

#include <stdio.h>
#include "grammar/grammar.h"

void grammar_output(FILE* output_file, struct grammar* grammar);
void automata_output(FILE* output_file, struct grammar* grammar);

#endif

