// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "ast.h"
#include "decompilation.h"

extern FILE *yyin;

AstNode *ast_root;

void yyparse();
void initMe();

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("call: ./etapa3 source.txt output.txt\n");
        exit(1);
    }

    if (0==(yyin = fopen(argv[1],"r"))) {
        printf("Cannot open file %s... \n",argv[1]);
        exit(1);
    }

    initMe();
    yyparse();
    ast_print(ast_root, 0);
    decompile(ast_root, argv[2]);
    print_hash();

    exit(0);
}

