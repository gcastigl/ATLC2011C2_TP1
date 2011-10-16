#include <stdlib.h>
#include <stdio.h>

#include "logic/grammar.h"
#include "lexers/lexers.h"
#include "output/output.h"

static void usage(char* filename) {
    printf("Usage: \n %s [file]\n\n", filename);
    exit(1);
}

int main(int argc, char** argv) {

    if (argc < 2) {
        usage(argv[0]);
    }

    int len = strlen(argv[1]);
    char* dot = ".dot";
    char* gc = ".gc";

    struct grammar* g;
    struct automata* a;

    if (strcmp(dot, argv[1]+len-4) == 0) {
        
        a = parse_automata_file(argv[1]);

        // 1. Símbolos terminales.
        printf("Terminal symbols: \n\t");
        for (int i = 0; i < a->number_chars; i++) {
            if (i != 0) printf(", ");
            printf("%c", a->chars[i]);
        }

        // 2. Estados.
        printf("\n\nStates: \n\t{");
        for (int i = 0; i < a->number_states; i++) {
            if (i != 0) printf(", ");
            printf("q%c", a->states[i]);
        }

        // 3. Estado inicial.
        printf("\n\nInitial state: q%c\n", a->states[0]);

        // 4. Conjunto de estados finales.
        printf("\nFinal states = {");
        bool first = true;
        for (int i = 0; i < a->number_states; i++) {
            if (a->final_state[i]) {
                if (!first) printf(", ");
                printf("q%c", a->states[i]);
            }
        }

        // 5. Tabla de la función de transición.
        // TODO
        print_ascii_table_for_automata(a);

        // 6. La especificación completa de la gramática equivalente.

        g = automata_to_grammar(a);

        char filename[255];
        memset(filename, 0, 255);
        strcpy(filename, argv[1]);
        strcpy(filename+len-3, "gr\0");
        filename[len-1] = '\0';
        FILE* file = fopen(argv[1], "w");
        grammar_output(file, g);

        destroy_grammar(g);
        free(a);
    }

    else if (strcmp(gc, argv[1]+len-3) == 0) {

        g = parse_grammar_file(argv[1]);

        // 1) Símbolos terminales
        printf("Terminal symbols:\n\t");
        for (int i = 0; i < g->number_symbols; i++) {
            if (g->symbols[i].terminal) {
                if (i != 0) printf(", ");
                printf("%c", g->symbols[i].representation);
            }
        }
        
        // 2) Símbolos no terminales
        printf("\n\nNon-terminal symbols:\n\t");
        for (int i = 0; i < g->number_symbols; i++) {
            if (!g->symbols[i].terminal) {
                if (i != 0) printf(", ");
                printf("%c", g->symbols[i].representation);
            }
        }

        // 3) Símbolo inicial.
        printf("\n\nDistinguished symbol: %c", g->distinguished_symbol);

        // 4) Si la gramática es válida.
        // ... a little late for that

        // 5) Si la gramática es regular (en caso de que lo sea,
        //    si es regular a derecha o a izquierda).
        if (g->alignment == RIGHT_ALIGNED) {
            printf("\n\nRight-Aligned grammar");
        } else if (g->alignment == LEFT_ALIGNED) {
            printf("\n\nLeft-Aligned grammar");
        } else {
            printf("\n\nUndefined alignment");
        }

        // 6) Un gráfico del automata finito equivalente.

        char filename[255];
        memset(filename, 0, 255);
        strcpy(filename, argv[1]);
        strcpy(filename+len-2, "png");
        FILE* file = fopen(argv[1], "w");
        struct grammar* right = as_right_normal_form(g); 
        automata_output(file, right);

        destroy_grammar(right);
        destroy_grammar(g);
    }

    else {
        usage(argv[0]);
    }

    return 0;
}


void print_ascii_table_for_automata(struct automata* a) {
    return;
}

