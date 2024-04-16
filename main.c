// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdio.h>
#include <stdlib.h>

#include "hash.h"

extern FILE *yyin;

void yyparse();
void initMe();

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("call: ./etapa2 input.txt\n");
        exit(1);
    }

    if (0==(yyin = fopen(argv[1],"r"))) {
        printf("Cannot open file %s... \n",argv[1]);
        exit(1);
    }

    initMe();
    yyparse();
    print_hash();

    exit(0);
}

