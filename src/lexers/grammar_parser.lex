%{
#include <stdlib.h>
#include <ctype.h>
#include "grammar/grammar.h"
%}
%START	BEG,NT, T, ST, PROD, PRODBEG, END
%%
%{
	BEGIN NT;
	int i;
	int nt_used[30] = {0};
	int t_used[30] = {0};
	char curr;
%}

<BEG>G[a-ZA-Z0-9]*[\ \n\t]*\(	{ BEGIN NT;}

<NT>\{( *[A-Z] *,)*\ *[A-Z]\ *\},	{
									for( i = 0; i < yylength ; i ++ ) {
										curr = yytext[i];
										if( 'A' <= curr && curr <= 'Z') {
											if( nt_used[curr - 'A'] == 0) {
												nt_used[curr - 'A'] = 1;
												add_terminal_symbol(g,curr);
											}
											else{
												//error
											}
										}
									}
									BEGIN T;
								}
<T>\{( *[a-z] *,)*\ *[a-z]\ *\},	{
									for( i = 0; i < yylength ; i ++ ) {
										curr = yytext[i];
										if( 'a' <= curr && curr <= 'z') {
											if( t_used[curr - 'a'] == 0) {
												t_used[curr - 'a'] = 1;
												add_non_terminal_symbol(g,curr);
											}
											else{
												//error
											}
										}
									}
									BEGIN ST;
								}
<ST>[A-Z]\ *,	{
					curr = yytext[0];
					if(nt_used[curr-'A']){
						set_distinguished(g, curr);
					}
					else{
						//error
					}
					BEGIN PROD;
				}
<PROD>\{	{
				BEGIN PRODBEG;
			}

<PRODBEG>[A-Z]\ *->( *([a-z][A-Z]?|[A-Z][a-z]|\\) *\|)*\ *([a-z][A-Z]?|[A-Z][a-z]|\\)\ *,	{

	int prodl = yytext[0];
	for( i = 1 ; i < yylength ; i ++ ) {
		curr = yytext[i];
		if(curr == '|'){
			//proxima produccion
		}

	}
}

<PRODBEG>\}	{BEGIN END;}
<END>\(	{//todo ok}
	;
}





%%
static int yywrap(void) {
	return 1;
}

struct grammar* parse_grammar_file(char* filename) {

    struct grammar* g = create_grammar();

    yyin = fopen(filename, "r");
    yylex();

    return g;
}

