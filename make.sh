#! /bin/sh

# Cleanup
rm -rf bin
mkdir bin
cd ./bin

# Compile lex files
flex ../src/lexers/automata_parser.lex -o automata_parser.c
flex ../src/lexers/grammar_parser.lex -o grammar_parser.c
gcc -oparser *.c -lfl

# Compile other C sources
# TODO

# Link all together
# TODO
