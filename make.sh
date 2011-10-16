#! /bin/sh

# Cleanup
rm -f TEST
rm -f run
rm -rf bin
mkdir bin
cd ./bin

# variables
FLAGS='-Wall -pedantic -std=c99'
INCLUDE='../src/'

# Compile lex files
touch automata_parser.c
touch grammar_parser.c
flex -o grammar_parser.c ../src/lexers/grammar_parser.lex

# Compile and link with other C sources
gcc $FLAGS -I$INCLUDE *.c ../src/logic/right_grammar.c ../src/logic/grammar.c ../src/main.c ../src/output/grammar_out.c ../src/output/automata_out.c ../src/logic/automata_to_grammar.c -lfl -o ../run


