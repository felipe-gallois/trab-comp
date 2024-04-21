// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "ast.h"

AstNode *ast_create(int type, HashEntry *symbol, AstNode* c0, AstNode* c1,
        AstNode* c2, AstNode* c3) {
    AstNode *new_node = (AstNode*) malloc(sizeof(AstNode));
    new_node->type = type;
    new_node->symbol = symbol;
    new_node->children[0] = c0;
    new_node->children[1] = c1;
    new_node->children[2] = c2;
    new_node->children[3] = c3;
    return new_node;
}

void ast_print(AstNode *node, int level) {
    if (node == NULL)
        return;

    for (int i = 0; i < level; i++) {
        fprintf(stderr, "  ");
    }

    fprintf(stderr, "ast(");

    switch (node->type) {
        case AST_SYMBOL:
            fprintf(stderr, "AST_SYMBOL");
            break;
        default:
            fprintf(stderr, "AST_UNKNOWN");
    }

    if (node->symbol != NULL) {
        fprintf(stderr, ",");
        fprintf(stderr, "%s", node->symbol->string);
    }

    fprintf(stderr, "\n");

    for (int i = 0; i < MAX_CHILDREN; i++)
        ast_print(node->children[i], level + 1);
}
