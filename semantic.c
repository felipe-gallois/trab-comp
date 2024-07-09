// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"
#include "semantic.h"

int semantic_errors = 0;

int is_declaration(AstNode *node);
int is_redeclared(HashEntry *entry);
void set_hash_type_from_decl_node(HashEntry *entry, AstNode *decl_node);
void set_hash_datatype_from_type_node(HashEntry *entry, AstNode *type_node);
void print_redeclaration_error(char *identifier_name);
void print_uncaught_parser_error();

void check_and_set_declarations(AstNode *node) {
    AstNode *type_node; 
    AstNode *identifier_node;

    if (node == NULL)
        return;

    if (is_declaration(node)) {
        type_node = node->children[0];
        identifier_node = node->children[1];

        if (is_redeclared(identifier_node->symbol)) {
            print_redeclaration_error(identifier_node->symbol->string);
            semantic_errors++;
        }

        set_hash_type_from_decl_node(identifier_node->symbol, node);
        set_hash_datatype_from_type_node(identifier_node->symbol, type_node);
    }

    for (int i = 0; i < MAX_CHILDREN; i++)
        check_and_set_declarations(node->children[i]);
}

void check_undeclared() {
    semantic_errors += verify_undeclared();
}

int is_declaration(AstNode *node) {
    int node_type = node->type;

    return (node_type == AST_VAR_DECL) 
        || (node_type == AST_VEC_DECL)
        || (node_type == AST_VEC_DECL_DEF)
        || (node_type == AST_FUNC_DECL)
        || (node_type == AST_PARAM);
}

int is_redeclared(HashEntry *entry) {
    if (entry->type == SYMBOL_TK_IDENTIFIER)
        return 0;

    return 1;
}

void set_hash_type_from_decl_node(HashEntry *entry, AstNode *decl_node) {
    int node_type = decl_node->type;
    int type = SYMBOL_TK_IDENTIFIER;

    switch (node_type) {
        case AST_VAR_DECL:
        case AST_PARAM:
            type = SYMBOL_VARIABLE;
            break;
        case AST_VEC_DECL:
            type = SYMBOL_VECTOR;
            break;
        case AST_VEC_DECL_DEF:
            type = SYMBOL_VECTOR;
            break;
        case AST_FUNC_DECL:
            type = SYMBOL_FUNCTION;
            break;
        default:
            print_uncaught_parser_error();
            break;
    }

    entry->type = type;
}

void set_hash_datatype_from_type_node(HashEntry *entry, AstNode *type_node) {
    int node_type = type_node->type;
    int datatype = DATATYPE_UNKNOWN;

    switch (node_type) {
        case AST_INT:
            datatype = DATATYPE_INT;
            break;
        case AST_CHAR:
            datatype = DATATYPE_CHAR;
            break;
        case AST_FLOAT:
            datatype = DATATYPE_REAL;
            break;
        case AST_BOOL:
            datatype = DATATYPE_BOOL;
            break;
        default:
            print_uncaught_parser_error();
            break;
    }

    entry->datatype = datatype;
}

void print_redeclaration_error(char *identifier_name) {
    fprintf(stderr,
            "Semantic error: redeclaring identifier %s\n",
            identifier_name);
}

void print_uncaught_parser_error() {
    fprintf(stderr, "Parser bug\n");
}

