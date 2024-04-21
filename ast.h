// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef AST_H
#define AST_H

#include "hash.h"

#define MAX_CHILDREN 4

enum AstNodeType {
    AST_SYMBOL 
};

typedef struct AstNode {
    int type;
    HashEntry *symbol;
    struct AstNode *children[MAX_CHILDREN];
} AstNode;

AstNode *ast_create(int type, HashEntry *symbol, AstNode* c0, AstNode* c1,
        AstNode* c2, AstNode* c3);

void ast_print(AstNode *node, int level);

#endif
