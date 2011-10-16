#! /bin/sh

# Cleanup
rm TEST
rm run
rm -rf bin
mkdir bin
cd ./bin

# variables
FLAGS='-Wall -pedantic -std=c99'
INCLUDE='../src/'

# Compile lex files
touch automata_parser.c
touch grammar_parser.c
flex -o automata_parser.c ../src/lexers/automata_parser.lex
flex -o grammar_parser.c ../src/lexers/grammar_parser.lex

# Compile and link with other C sources
gcc $FLAGS -I$INCLUDE *.c ../src/grammar/right_grammar.c ../src/grammar/grammar.c ../src/main.c ../src/output/grammar_out.c ../src/output/automata_out.c -lfl -o ../run


