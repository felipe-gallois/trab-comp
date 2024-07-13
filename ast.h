// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef AST_H
#define AST_H

#include "hash.h"

#define MAX_CHILDREN 4

enum AstNodeType {
    AST_SYMBOL,
    AST_BOOL,
    AST_FLOAT,
    AST_INT,
    AST_CHAR,
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
    AST_VAR_EXP,
    AST_VEC_EXP,
    AST_FUNC_EXP,
    AST_ARGS_LIST,
    AST_LIT_LIST,
    AST_CMD_LIST,
    AST_VAR_ATTRIB,
    AST_VEC_ATTRIB,
    AST_RET,
    AST_EMPTY,
    AST_PRINT_STRING,
    AST_PRINT_LIT,
    AST_READ,
    AST_IF,
    AST_IF_ELSE,
    AST_WHILE,
    AST_BLOCK,
    AST_PAR,
    AST_PARAM,
    AST_PARAM_LIST,
    AST_FUNC_DECL,
    AST_VEC_DECL,
    AST_VEC_DECL_DEF,
    AST_VAR_DECL,
    AST_DECL_LIST
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
