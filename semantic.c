// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"

int semantic_errors = 0;

static HashEntry *var;

void match_datatype(int *datatype, int type);

void annotate_types(AstNode *node) {
    if (node == NULL)
        return;

    switch (node->type) {
        case AST_VAR_DECL:
            var = node->children[1]->symbol; 
            if (var->type == SYMBOL_TK_IDENTIFIER) {
                var->type = SYMBOL_VARIABLE;
                match_datatype(&(var->datatype), node->children[0]->type);
            } else {
                semantic_errors++;
                fprintf(stderr, "Semantic error: redeclaring identifier %s\n", var->string);
            }
            break;
        case AST_VEC_DECL:
            var = node->children[1]->symbol; 
            if (var->type == SYMBOL_TK_IDENTIFIER) {
                var->type = SYMBOL_VECTOR;
                match_datatype(&(var->datatype), node->children[0]->type);
            } else {
                semantic_errors++;
                fprintf(stderr, "Semantic error: redeclaring identifier %s\n", var->string);
            }
            break;
        case AST_VEC_DECL_DEF:
            var = node->children[1]->symbol; 
            if (var->type == SYMBOL_TK_IDENTIFIER) {
                var->type = SYMBOL_VECTOR;
                match_datatype(&(var->datatype), node->children[0]->type);
            } else {
                semantic_errors++;
                fprintf(stderr, "Semantic error: redeclaring identifier %s\n", var->string);
            }
            break;
        case AST_FUNC_DECL:
            var = node->children[1]->symbol; 
            if (var->type == SYMBOL_TK_IDENTIFIER) {
                var->type = SYMBOL_FUNCTION;
                match_datatype(&(var->datatype), node->children[0]->type);
            } else {
                semantic_errors++;
                fprintf(stderr, "Semantic error: redeclaring identifier %s\n", var->string);
            }
            break;
        default:
            break;
    }

    for (int i = 0; i < MAX_CHILDREN; i++)
        annotate_types(node->children[i]);
}

void match_datatype(int *datatype, int type) {
    switch (type) {
        case AST_INT:
            *datatype = DATATYPE_INT;
            break;
        case AST_CHAR:
            *datatype = DATATYPE_CHAR;
            break;
        case AST_FLOAT:
            *datatype = DATATYPE_REAL;
            break;
        case AST_BOOL:
            *datatype = DATATYPE_BOOL;
            break;
        default:
            break;
    }
}
