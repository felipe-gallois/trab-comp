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

void ast_print_node(AstNode *node, int level) {
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
        case AST_BOOL:
            fprintf(stderr, "AST_BOOL");
            break;
        case AST_FLOAT:
            fprintf(stderr, "AST_FLOAT");
            break;
        case AST_INT:
            fprintf(stderr, "AST_INT");
            break;
        case AST_CHAR:
            fprintf(stderr, "AST_CHAR");
            break;
        case AST_SUM:
            fprintf(stderr, "AST_SUM");
            break;
        case AST_SUB:
            fprintf(stderr, "AST_SUB");
            break;
        case AST_MUL:
            fprintf(stderr, "AST_MUL");
            break;
        case AST_DIV:
            fprintf(stderr, "AST_DIV");
            break;
        case AST_LESS:
            fprintf(stderr, "AST_LESS");
            break;
        case AST_GREATER:
            fprintf(stderr, "AST_GREATER");
            break;
        case AST_LE:
            fprintf(stderr, "AST_LE");
            break;
        case AST_GE:
            fprintf(stderr, "AST_GE");
            break;
        case AST_EQ:
            fprintf(stderr, "AST_EQ");
            break;
        case AST_DIF:
            fprintf(stderr, "AST_DIF");
            break;
        case AST_AND:
            fprintf(stderr, "AST_AND");
            break;
        case AST_OR:
            fprintf(stderr, "AST_OR");
            break;
        case AST_NOT:
            fprintf(stderr, "AST_NOT");
            break;
        case AST_VEC_EXP:
            fprintf(stderr, "AST_VEC_EXP");
            break;
        case AST_FUNC_EXP:
            fprintf(stderr, "AST_FUNC_EXP");
            break;
        case AST_ARGS_LIST:
            fprintf(stderr, "AST_ARGS_LIST");
            break;
        case AST_CMD_LIST:
            fprintf(stderr, "AST_CMD_LIST");
            break;
        case AST_VAR_ATTRIB:
            fprintf(stderr, "AST_VAR_ATTRIB");
            break;
        case AST_VEC_ATTRIB:
            fprintf(stderr, "AST_VEC_ATTRIB");
            break;
        case AST_RET:
            fprintf(stderr, "AST_RET");
            break;
        case AST_EMPTY:
            fprintf(stderr, "AST_EMPTY");
            break;
        case AST_PRINT:
            fprintf(stderr, "AST_PRINT");
            break;
        case AST_READ:
            fprintf(stderr, "AST_READ");
            break;
        case AST_IF:
            fprintf(stderr, "AST_IF");
            break;
        case AST_WHILE:
            fprintf(stderr, "AST_WHILE");
            break;
        case AST_BLOCK:
            fprintf(stderr, "AST_BLOCK");
            break;
        default:
            fprintf(stderr, "AST_UNKNOWN");
    }

    if (node->symbol != NULL) {
        fprintf(stderr, ",");
        fprintf(stderr, "%s", node->symbol->string);
    }

    fprintf(stderr, "\n");
}

void ast_print(AstNode *node, int level) {
    if (node == NULL)
        return;

    ast_print_node(node, level);

    for (int i = 0; i < MAX_CHILDREN; i++)
        ast_print(node->children[i], level + 1);
}
