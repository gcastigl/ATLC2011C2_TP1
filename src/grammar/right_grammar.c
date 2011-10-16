#include "grammar/grammar.h"

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

            if (prod->representation == symbol) {

                f(grammar, prod, data);

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

static void check_reachable(struct grammar* grammar, 
    struct production* production, bool* reach
){

    for (int s = 0; s <= 0; s++) {
        int repr = production->right_part[s].representation;
        reach[repr] = true;
    }
}

struct grammar* take_out_unreachable(struct grammar* source) {
    
    struct grammar* new = create_grammar();    

    bool reachable_symbol[MAX_SYMBOLS];
    memset(reachable_symbol, 0, MAX_SYMBOLS);

    production_bfs(source,
        (void*(*)(struct grammar*, void*, void*))check_reachable,
        reachable_symbol
    );

    // Copy all reachable symbols
    for (int i = 0; i < source->number_symbols; i++) {
        struct symbol* symbol = &(source->symbols[i]);

        if (reachable_symbol[(int)symbol->representation]) {
            add_symbol(new, false, symbol->representation);
        }
    }

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

                if (!reachable_symbol[(int)right_part[s]]) {
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

struct grammar* replace_lambda_with_M(struct grammar* source) {

    struct grammar* new = create_grammar();

    for (int i = 0; i < source->number_symbols; i++) {
        add_symbol(new, source->symbols[i].terminal,
            source->symbols[i].representation
        );
    }

    // This will be our 'M' symbol. We choose something not printable.
    add_symbol(new, false, '\t');

    set_distinguished_symbol(new, '\t');

    for (int i = 0; i < source->number_productions; i++) {

        char right_part[3] = {0, 0, 0};

        right_part[0] = source->productions[i].right_part[0].representation;
        right_part[1] = source->productions[i].right_part[1].representation;

        if (right_part[0] == '\\') {
            right_part[0] = '\t';
        }

        add_production(new, source->productions[i].left_part.representation,
            right_part
        );
    }

    return new;
}

static bool has_unitary_productions(struct grammar* source) {

    for (int i = 0; i < source->number_productions; i++) {

        if (source->productions[i].right_part[0].terminal == false &&
            source->productions[i].right_part[1].symbol != true
        ){
            return true;
        }
    }
    return false;
}

static struct grammar* take_out_unitary_productions(struct grammar* source) {


}

static struct grammar* reverse_productions(struct grammar* source) {

    struct grammar* new = create_grammar();

    for (int i = 0; i < source->number_symbols; i++) {
        add_symbol(new, source->symbols[i].terminal,
            source->symbols[i].representation
        );
    }

    set_distinguished_symbol(new, '\t');

    // FIXME: me parece que me estoy olvidando algo acá
    for (int i = 0; i < source->number_productions; i++) {
        
        char new_production[4] = {0, 0, 0, 0};
        new_production[0] = source->productions[i].left_part.representation;
        new_production[1] = source->productions[i].right_part[0].representation;
        new_production[2] = source->productions[i].right_part[1].representation;

        if (source->productions[i].right_part[0].terminal == false &&
            source->productions[i].right_part[1].terminal == true
        ){
            char temp = new_production[0];
            new_production[0] = new_production[1];
            new_production[1] = new_production[2];
            new_production[2] = temp; 
        }

        add_production(new, new_production[0], new_production+1);
    }

    return new;
}

struct grammar* to_right_normal_form(struct grammar* source) {

    source = take_out_unreachable(source);    

    struct grammar* new = replace_lambda_with_M(source);
    
    while(has_unitary_productions(new)) {

        struct grammar* replace = take_out_unitary_productions(new);

        destroy_grammar(new);

        new = replace;
    }

    struct grammar* reverse = reverse_productions(new);

    destroy_grammar(new);

    new = take_out_unreachable(reverse);
    
    while(has_unitary_productions(new)) {

        struct grammar* replace = take_out_unitary_productions(new);

        destroy_grammar(new);

        new = replace;
    }

    struct grammar* ret = take_out_unreachable(new);

    destroy_grammar(new);

    return ret;
}

