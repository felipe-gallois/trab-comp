// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"
#include "semantic.h"

int semantic_errors = 0;

int is_declaration(AstNode *node);
int is_redeclared(HashEntry *entry);
int is_char_or_int(enum DataType data_type);

void set_hash_type_from_decl_node(HashEntry *entry, AstNode *decl_node);
void set_hash_datatype_from_type_node(HashEntry *entry, AstNode *type_node);

enum DataType eval_symbol(HashEntry *symbol);
enum DataType eval_arith_op(enum DataType t1, enum DataType t2);
enum DataType eval_test_op(enum DataType t1, enum DataType t2);

void print_redeclaration_error(char *identifier_name);
void print_undeclared_error(char *identifier_name);
void print_op_type_error();
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

enum DataType check_nodes(AstNode *node) {
    if (node == NULL)
        return DATATYPE_UNKNOWN;

    enum DataType children_eval[MAX_CHILDREN] = {DATATYPE_UNKNOWN};

    for (int i = 0; i < MAX_CHILDREN; i++)
        children_eval[i] = check_nodes(node->children[i]);

    enum DataType node_eval = DATATYPE_UNKNOWN;

    switch (node->type) {
        case AST_SYMBOL:
            node_eval = eval_symbol(node->symbol);
            break;
        case AST_SUM:
        case AST_SUB:
        case AST_MUL:
        case AST_DIV:
            node_eval = eval_arith_op(children_eval[0], children_eval[1]);
            break;
        case AST_LESS:
        case AST_GREATER:
        case AST_LE:
        case AST_GE:
        case AST_EQ:
        case AST_DIF:
            node_eval = eval_test_op(children_eval[0], children_eval[1]);
            break;
        default:
            break;
    }

    return node_eval;
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

int is_char_or_int(enum DataType data_type) {
    return (data_type == DATATYPE_CHAR) || (data_type == DATATYPE_INT);
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

enum DataType eval_symbol(HashEntry *symbol) {
    if (symbol == NULL) {
        print_uncaught_parser_error();
        return DATATYPE_UNKNOWN;
    }

    enum DataType eval = DATATYPE_UNKNOWN;

    switch (symbol->type) {
        case SYMBOL_VARIABLE:
        case SYMBOL_VECTOR:
        case SYMBOL_FUNCTION:
            eval = symbol->datatype;
            break;
        case SYMBOL_LIT_INT:
            eval = DATATYPE_INT;
            break;
        case SYMBOL_LIT_CHAR:
            eval = DATATYPE_CHAR;
            break;
        case SYMBOL_LIT_REAL:
            eval = DATATYPE_REAL;
            break;
        case SYMBOL_LIT_TRUE:
        case SYMBOL_LIT_FALSE:
            eval = DATATYPE_BOOL;
            break;
        case SYMBOL_LIT_STRING:
            eval = DATATYPE_STRING;
            break;
        default:
            break;
    }

    if (eval == DATATYPE_UNKNOWN) {
        print_undeclared_error(symbol->string);
        semantic_errors++;
    }

    return eval;
}

enum DataType eval_arith_op(enum DataType t1, enum DataType t2) {
    enum DataType eval = DATATYPE_UNKNOWN;

    if (is_char_or_int(t1) && is_char_or_int(t2)) {
        eval = DATATYPE_INT;
    } else if (t1 == DATATYPE_REAL && t2 == DATATYPE_REAL) {
        eval = DATATYPE_REAL;
    } else {
        print_op_type_error();
        semantic_errors++;
    }

    return eval;
}

enum DataType eval_test_op(enum DataType t1, enum DataType t2) {
    enum DataType eval = DATATYPE_UNKNOWN;

    if ((is_char_or_int(t1) && is_char_or_int(t2))
            || (t1 == DATATYPE_REAL && t2 == DATATYPE_REAL)) {
        eval = DATATYPE_BOOL;
    } else {
        print_op_type_error();
        semantic_errors++;
    }

    return eval;
}

void print_redeclaration_error(char *identifier_name) {
    fprintf(stderr,
            "Semantic error: redeclaring identifier %s\n",
            identifier_name);
}

void print_undeclared_error(char *identifier_name) {
    fprintf(stderr,
            "Semantic error: identifier %s undeclared\n",
            identifier_name);
}

void print_op_type_error() {
    fprintf(stderr,
            "Semantic error: operation has invalid operand types\n");
}

void print_uncaught_parser_error() {
    fprintf(stderr, "Parser bug\n");
}

