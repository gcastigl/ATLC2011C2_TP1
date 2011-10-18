#include <string.h>

#include "logic/grammar.h"

/**
 * Auxiliary function that copies all the symbols from one grammar to the other.
 *
 * @param source the original grammar
 * @param dest   the new grammar
 */
static void copy_symbols(struct grammar* source, struct grammar* dest) {

    for (int i = 0; i < source->number_symbols; i++) {
        add_symbol(dest, source->symbols[i].terminal,
            source->symbols[i].representation
        );
    }

}

/**
 * Auxiliary function that copies symbols from one grammar to the other, given 
 * that mask[(int)symbol_representation] is true.
 *
 * @param source the source grammar
 * @param dest   the grammar to put symbols in
 * @param mask   which symbols should be copied
 */
static void copy_symbols_under_mask(struct grammar* source,
                                    struct grammar* dest, bool* mask)
{
    for (int i = 0; i < source->number_symbols; i++) {
        
        if (mask[(int)source->symbols[i].representation]) {

            add_symbol(dest, source->symbols[i].terminal,
                source->symbols[i].representation);
        }
    }
}

/**
 * Copies a given production into another grammar.
 * 
 * @param grammar    a grammar to put the new production into
 * @param production a production to copy into the grammar
 */
static void add_production_copy(struct grammar* grammar,
                                struct production* production)
{
    char prod[3] = {0, 0, 0};
    prod[0] = production->right_part[0].representation;
    prod[1] = production->right_part[1].representation;
    add_production(grammar, production->left_part.representation, prod);
}

/**
 * Iterates over the symbols of a grammar, traversing them as they appear in 
 * order of the productions. Only productive productions are traversed.
 *
 * Example: if the grammar were to be {S -> A | B, A->C, D->E}, the
 * productions would be traversed in this order: S->A, S->B, A->C. D->E is
 * never checked.
 *
 * @param grammar the grammar to work on
 * @param f       a function to be called on each production
 * @param data    a pointer that will be passed to f to store data
 */
static void production_bfs(struct grammar* grammar,
    void* (*f) (struct grammar*, void*, void*), void* data)
{

    // Binary array stores whether symbol was visited
    bool done[255];
    memset(done, 0, 255);

    // Symbol is going to be the symbol we're currently seeing
    char symbol = grammar->distinguished_symbol;
    done[(int)symbol] = true;

    // Setup the queue
    char queue[255];
    memset(queue, 0, 255);
    int begin = 0;
    int end = 0;
    queue[end++] = symbol;

    // Start BFS
    while (begin < end) {
        
        symbol = queue[begin++];

        for (int i = 0; i < grammar->number_productions; i++) {

            struct production* prod = &(grammar->productions[i]);

            if (prod->left_part.representation == symbol) {

                // Call this function for each represented symbol
                f(grammar, prod, data);

                // Add new symbols to the queue
                for (int s = 0; s <= 1; s++) {
                    if (prod->right_part[s].terminal == false &&
                        done[(int)prod->right_part[s].representation] == false
                    ) {
                        queue[end++] = prod->right_part[s].representation;
                        done[(int)prod->right_part[s].representation] = true;
                    }
                }
            }
        }
    }
}

/**
 * This is an auxiliary function to be called when the function
 * take_out_unreachable does a BFS traversal to see what symbols are worth
 * keeping.
 *
 * It marks terminal and non-terminal symbols as 'reachables' in a auxiliary
 * array that is passed as a parameter to the bfs, and is a output paramater
 *
 * @param grammmar   the grammar (unused, for BFS signature compatibility)
 * @param production the production that got seen in the BFS algorithm
 * @param reach      output parameter, will only have the reachable (both
 *                   terminal and non-terminal) symbols set to true
 */
static void check_reachable(struct grammar* grammar, 
    struct production* production, bool* reach
){

    for (int s = 0; s <= 1; s++) {
        int repr = production->right_part[s].representation;
        reach[repr] = true;
    }
}

/**
 * Creates a new grammar without unreachable symbols
 *
 * @param source the original grammar
 */
struct grammar* take_out_unreachable(struct grammar* source) {
    
    struct grammar* new = create_grammar();    

    bool reachable_symbol[MAX_SYMBOLS];
    memset(reachable_symbol, 0, MAX_SYMBOLS);

    production_bfs(source,
        (void*(*)(struct grammar*, void*, void*))check_reachable,
        reachable_symbol
    );

    reachable_symbol[(int)source->distinguished_symbol] = true;
    for (int i = 0; i < source->number_symbols; i++) {
        if (source->symbols[i].terminal == true) {
            reachable_symbol[(int)source->symbols[i].representation] = true;
        }
    }

    // Copy all reachable symbols
    copy_symbols_under_mask(source, new, reachable_symbol);

    // Setup distinguished symbol
    set_distinguished_symbol(new, source->distinguished_symbol);

    // Copy reachable productions
    for (int i = 0; i < source->number_productions; i++) {
        
        struct production* prod = &(source->productions[i]);

        if (reachable_symbol[(int)prod->left_part.representation]) {
            
            char right_part[3] = {0, 0, 0};
            bool selectable = true;

            for (int s = 0; s <= 1; s++) {
                right_part[s] = prod->right_part[s].representation;

                if (right_part[s] != '\0' &&
                    !reachable_symbol[(int)right_part[s]])
                {
                    selectable = false;
                }
            }

            if (selectable) {
                add_production(new, prod->left_part.representation, right_part);
            }
        }
    }

    return new;
}

/**
 * Creates a new grammar where all the lambda productions are replaced with 
 * a new symbol. We choosed '\t' because it's whitespace and not printable.
 *
 * Warning: This doesn't delete unreachable or improductive productions.
 *
 * @param source the original grammar
 * @return       a new grammar with no productions of the form A->λ unless
 *               A is the distinguished symbol
 */
static struct grammar* replace_lambda_with_M(struct grammar* source) {

    struct grammar* new = create_grammar();
    copy_symbols(source, new);

    // This will be our 'M' symbol. We choose something not printable.
    add_symbol(new, false, '\t');

    set_distinguished_symbol(new, source->distinguished_symbol);

    for (int i = 0; i < source->number_productions; i++) {

        char right_part[3] = {0, 0, 0};

        right_part[0] = source->productions[i].right_part[0].representation;
        right_part[1] = source->productions[i].right_part[1].representation;

        if (right_part[0] == '\\') {
            right_part[1] = '\\';
            right_part[0] = '\t';
        }
        else if (right_part[1] == '\0') {
            right_part[1] = right_part[0];
            right_part[0] = '\t';
        }

        add_production(new, source->productions[i].left_part.representation,
            right_part
        );
    }

    char lambda_transition[2] = { '\\', '\0' };
    add_symbol(new, true, '\\');
    add_production(new, '\t', lambda_transition);

    return new;
}

/**
 * Checks whether a grammar has unitary productions
 *
 * @param source the grammar to work with
 * @return       true if the grammar has a unitary production
 */
static bool has_unitary_productions(struct grammar* source) {

    for (int i = 0; i < source->number_productions; i++) {

        if (source->productions[i].right_part[0].terminal == false &&
            source->productions[i].right_part[1].representation == '\0'
        ){
            return true;
        }
    }
    return false;
}

/**
 * Creates a new grammar with one less unitary production.
 *
 * @param source the original grammar
 * @return       a new grammar with one less unitary production
 */
static struct grammar* take_out_unitary_productions(struct grammar* source) {

    struct grammar* new = create_grammar();

    copy_symbols(source, new);

    bool not_replaced = true;

    for (int i = 0; i < source->number_productions; i++) {
        

        if (not_replaced &&
            source->productions[i].right_part[0].terminal == false &&
            source->productions[i].right_part[1].representation == '\0'
        ){
            for (int j = 0; j < source->number_productions; j++) {

                if (source->productions[j].left_part.representation ==
                    source->productions[i].right_part[0].representation
                ){
                    char new_rightpart[3] = {0, 0, 0};

                    new_rightpart[0] =
                        source->productions[j].right_part[0].representation;
                    new_rightpart[1] =
                        source->productions[j].right_part[1].representation;
                    add_production(new,
                        source->productions[i].left_part.representation,
                        new_rightpart
                    );
                }
            }
            not_replaced = false;

        } else {
            
            add_production_copy(new, &(source->productions[i]));
        }
    }

    return new;
}

/**
 * Uses the A->Ba => B->aA rule to create a new grammar that may be in right 
 * normal form (unproductive and unreachable productions may appear).
 *
 * @param source the grammar to be transformed
 * @return       a grammar without productions of the form A->Ba
 */
static struct grammar* reverse_productions(struct grammar* source) {

    struct grammar* new = create_grammar();

    copy_symbols(source, new);

    set_distinguished_symbol(new, '\t');

    for (int i = 0; i < source->number_productions; i++) {
        
        char new_production[4] = {0, 0, 0, 0};
        new_production[0] = source->productions[i].left_part.representation;
        new_production[1] = source->productions[i].right_part[0].representation;
        new_production[2] = source->productions[i].right_part[1].representation;

        if (new_production[1] == source->distinguished_symbol) {
            new_production[1] = '\t';
        }

        if (source->productions[i].right_part[0].terminal == false &&
            source->productions[i].right_part[1].terminal == true
        ){
            char temp = new_production[0];
            new_production[0] = new_production[1];
            if( source->productions[i].right_part[1].representation != '\\' ){
                new_production[1] = new_production[2];
                new_production[2] = temp;
            }
            else{
                new_production[1] = temp;
                new_production[2] = 0;
            }
        }
        add_production(new, new_production[0], new_production+1);
    }

    return new;
}

/**
 * Checks whether the grammar has unproductive productions.
 *
 * @param grammar the grammar to work with
 * @return        true if the grammar has unproductive productions
 */
static bool has_unproductive_productions(struct grammar* grammar) {

    for (int i = 0; i < grammar->number_symbols; i++) {

        if (grammar->symbols[i].terminal == false) {

            bool unproductive = true;
            for (int j = 0; j < grammar->number_productions; j++) {
            
                if (grammar->productions[j].left_part.representation ==
                    grammar->symbols[i].representation ||
                    grammar->productions[j].right_part[0].representation ==
                    grammar->symbols[i].representation ||
                    grammar->productions[j].right_part[1].representation ==
                    grammar->symbols[i].representation
                ){
                    unproductive = false;
                }
            }

            if (unproductive) {
                return true;
            }
        }
    }
    return false;
}

/**
 * Delete unproductive productions from grammar. A unproductive production is
 * one of the form A->B where B doesn't lead to anything.
 *
 * @param source the grammar to work with
 * @return       a new grammar with less (not warranty of all deleted)
 *               unproductive productions
 */
static struct grammar* take_out_unproductive_production(struct grammar* source)
{
    struct grammar* new = create_grammar();

    bool char_map[255];
    memset(char_map, 0, 255);

    for (int i = 0; i < source->number_symbols; i++) {
        
        if (!source->symbols[i].terminal) {
            
            for (int j = 0; j < source->number_productions; j++) {
                
                if (source->productions[j].left_part.representation ==
                    source->symbols[i].representation ||
                    source->productions[j].right_part[0].representation ==
                    source->symbols[i].representation ||
                    source->productions[j].right_part[1].representation ==
                    source->symbols[i].representation
                ){

                    char_map[(int)source->symbols[i].representation] = true;
                }
            }
        } else {

            char_map[(int)source->symbols[i].representation] = true;
        }
    }

    set_distinguished_symbol(new, source->distinguished_symbol);

    copy_symbols_under_mask(source, new, char_map);

    for (int i = 0; i < source->number_productions; i++) {

        if (char_map[(int)source->productions[i].left_part.representation]) {

            add_production_copy(new, &(source->productions[i]));
        }
    }

    return new;
}

/**
 * Translates a grammar and takes it to the right normal form. To do this, the
 * algorithm has to:
 * 
 *  -> Change all lambda transitions of form A -> λ to A -> M
 *  -> Delete unitary productions
 *  -> Delete unproductive productions
 *  -> Use the rule A -> Ba => B -> aA to transform left-formed productions
 *  -> Again, check for unitary and unproductive productions.
 *
 * This function creates a new grammar. 
 *
 * @param source the grammar to work with
 * @return       a grammar in the right normal form
 */
struct grammar* as_right_normal_form(struct grammar* source) {

    source = take_out_unreachable(source);    

    while (has_unitary_productions(source)) {
        struct grammar* replace = take_out_unitary_productions(source);
        destroy_grammar(source);
        source = replace;
    }

    while (has_unproductive_productions(source)) {
        struct grammar* replace = take_out_unproductive_production(source);
        destroy_grammar(source);
        source = replace;
    }

    struct grammar* new = replace_lambda_with_M(source);
    destroy_grammar(source);
    struct grammar* reverse = reverse_productions(new);
    destroy_grammar(new);
    new = take_out_unreachable(reverse);
    destroy_grammar(reverse);
    
    while (has_unitary_productions(new)) {
        struct grammar* replace = take_out_unitary_productions(new);
        destroy_grammar(new);
        new = replace;
    }

    while (has_unproductive_productions(new)) {
        struct grammar* replace = take_out_unproductive_production(new);
        destroy_grammar(new);
        new = replace;
    }

    return new;
}

