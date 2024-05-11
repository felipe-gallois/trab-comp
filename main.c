// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdio.h>
#include <stdlib.h>

#include "hash.h"
#include "ast.h"
#include "semantic.h"
#include "decompilation.h"

extern int semantic_errors;
extern FILE *yyin;

AstNode *ast_root;

void yyparse();
void initMe();

int main(int argc, char** argv) {
    if (argc < 3) {
        printf("call: ./etapa4 source.txt output.txt\n");
        exit(1);
    }

    if (0==(yyin = fopen(argv[1],"r"))) {
        printf("Cannot open file %s... \n",argv[1]);
        exit(1);
    }

    initMe();
    yyparse();
    ast_print(ast_root, 0);
    annotate_types(ast_root);
    decompile(ast_root, argv[2]);
    print_hash();

    if (semantic_errors > 0)
        exit(4);

    exit(0);
}

