// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef AST_H
#define AST_H

#include "hash.h"

#define MAX_CHILDREN 4

enum AstNodeType {
    AST_SYMBOL,
    AST_SUM,
    AST_SUB,
    AST_MUL,
    AST_DIV,
    AST_LESS,
    AST_GREATER,
    AST_LE,
    AST_GE,
    AST_EQ,
    AST_DIF,
    AST_AND,
    AST_OR,
    AST_NOT,
    AST_VEC_EXP,
    AST_FUNC_EXP,
    AST_ARGS_LIST,
    AST_CMD_LIST,
    AST_VAR_ATTRIB,
    AST_VEC_ATTRIB,
    AST_RET,
    AST_IF,
    AST_WHILE,
    AST_BLOCK
};

typedef struct AstNode {
    int type;
    HashEntry *symbol;
    struct AstNode *children[MAX_CHILDREN];
} AstNode;

AstNode *ast_create(int type, HashEntry *symbol, AstNode* c0, AstNode* c1,
        AstNode* c2, AstNode* c3);

void ast_print_node(AstNode *node, int level);

void ast_print(AstNode *node, int level);

#endif
