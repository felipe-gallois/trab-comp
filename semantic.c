// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"
#include "semantic.h"

int semantic_errors = 0;

int is_declaration(AstNode *node);
int is_redeclared(HashEntry *entry);
int is_vector_decl(AstNode *node);
int is_func_decl(AstNode *node);
int is_char_or_int(enum DataType data_type);
int is_compatible(enum DataType t1, enum DataType t2);
int is_within_bounds(AstNode *identifier, AstNode *index);

void set_hash_type_from_decl_node(HashEntry *entry, AstNode *decl_node);
void set_hash_datatype_from_type_node(HashEntry *entry, AstNode *type_node);
void set_hash_capacity(HashEntry *identifier, HashEntry *capacity);
void set_hash_param_from_param_list_node(AstNode *param_list, TypeList **insert_place);

enum DataType eval_symbol(HashEntry *symbol);
enum DataType eval_arith_op(enum DataType t1, enum DataType t2);
enum DataType eval_test_op(enum DataType t1, enum DataType t2);
enum DataType eval_bool_test_op(enum DataType t1, enum DataType t2);
enum DataType eval_not_op(enum DataType type);
enum DataType eval_vec_exp(AstNode *identifier, AstNode *index);
enum DataType eval_func_exp(AstNode *identifier, AstNode *args_list);
enum DataType eval_lit_list(enum DataType t1, enum DataType t2);
enum DataType eval_var_attrib(enum DataType identifier_type, enum DataType expr_type);
enum DataType eval_vec_attrib(AstNode *identifier, AstNode *index, enum DataType expr_datatype);
enum DataType eval_par(enum DataType type);

void print_redeclaration_error(char *identifier_name);
void print_undeclared_error(char *identifier_name);
void print_type_error();
void print_arg_size_too_small_error();
void print_arg_size_too_big_error();
void print_out_of_bounds_error();
void print_uncaught_parser_error();

void check_and_set_declarations(AstNode *node) {
    AstNode *type_node; 
    AstNode *identifier_node;
    AstNode *extra_node;

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

        if (is_vector_decl(node)) {
            extra_node = node->children[2];
            set_hash_capacity(identifier_node->symbol, extra_node->symbol);
        } else if (is_func_decl(node)) {
            extra_node = node->children[2];
            set_hash_param_from_param_list_node(
                    extra_node,
                    &(identifier_node->symbol->parameters)
            ); 
        }
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
        case AST_AND:
        case AST_OR:
            node_eval = eval_bool_test_op(children_eval[0], children_eval[1]);
            break;
        case AST_NOT:
            node_eval = eval_not_op(children_eval[0]);
            break;
        case AST_VEC_EXP:
            node_eval = eval_vec_exp(node->children[0], node->children[1]);
            break;
        case AST_FUNC_EXP:
            node_eval = eval_func_exp(node->children[0], node->children[1]);
            break;
        case AST_LIT_LIST:
            node_eval = eval_lit_list(children_eval[0], children_eval[1]);
            break;
        case AST_VAR_ATTRIB:
            node_eval = eval_var_attrib(children_eval[0], children_eval[1]);
            break;
        case AST_VEC_ATTRIB:
            node_eval = eval_vec_attrib(
                    node->children[0],
                    node->children[1],
                    children_eval[2]
            );
            break;
        case AST_PAR:
            node_eval = eval_par(children_eval[0]);
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

int is_vector_decl(AstNode *node) {
    return (node->type == AST_VEC_DECL)
        || (node->type == AST_VEC_DECL_DEF);
}

int is_func_decl(AstNode *node) {
    return (node->type == AST_FUNC_DECL);
}

int is_char_or_int(enum DataType data_type) {
    return (data_type == DATATYPE_CHAR) || (data_type == DATATYPE_INT);
}

int is_compatible(enum DataType t1, enum DataType t2) {
    return (t1 == t2)
        || (is_char_or_int(t1) && is_char_or_int(t2));
}

int is_within_bounds(AstNode *identifier, AstNode *index) {
    // FIXME: index could also be a char
    unsigned long idx = strtoul(index->symbol->string, NULL, 10);

    if (idx < identifier->symbol->capacity) {
        return 1;
    } else {
        return 0;
    }
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

void set_hash_capacity(HashEntry *identifier, HashEntry *capacity) {
    // FIXME: capacity could also be a char
    unsigned long cap = strtoul(capacity->string, NULL, 10); 
    identifier->capacity = cap;
}

void set_hash_param_from_param_list_node(
        AstNode *param_list,
        TypeList **insert_place) {
    if (param_list != NULL) {
        TypeList *type_node = (TypeList*) malloc(sizeof(TypeList));

        type_node->next = NULL;

        // switch parameter's type
        switch (param_list->children[0]->children[0]->type) {
            case AST_CHAR:
                type_node->type = DATATYPE_CHAR;
                break;
            case AST_INT:
                type_node->type = DATATYPE_INT;
                break;
            case AST_FLOAT:
                type_node->type = DATATYPE_REAL;
                break;
            case AST_BOOL:
                type_node->type = DATATYPE_BOOL;
                break;
        }

        *insert_place = type_node; 

        // recursion
        set_hash_param_from_param_list_node(
                param_list->children[1],
                &(type_node->next)
        );
    }
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
        print_type_error();
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
        print_type_error();
        semantic_errors++;
    }

    return eval;
}

enum DataType eval_bool_test_op(enum DataType t1, enum DataType t2) {
    enum DataType eval = DATATYPE_UNKNOWN;

    if (t1 == DATATYPE_BOOL && t2 == DATATYPE_BOOL) {
        eval = DATATYPE_BOOL;
    } else {
        print_type_error();
        semantic_errors++;
    }

    return eval;
}

enum DataType eval_not_op(enum DataType type) {
    enum DataType eval = DATATYPE_UNKNOWN;

    if (type == DATATYPE_BOOL) {
        eval = DATATYPE_BOOL;
    } else {
        print_type_error();
        semantic_errors++;
    }

    return eval;
}

enum DataType eval_vec_exp(AstNode *identifier, AstNode *index) {
    enum SymbolType index_type = index->symbol->type;

    if ((index_type == SYMBOL_LIT_INT) || (index_type == SYMBOL_LIT_CHAR)) {
        if (!is_within_bounds(identifier, index)) {
            print_out_of_bounds_error();
            semantic_errors++;
        }
    } else {
        print_type_error();
        semantic_errors++;
    }

    return identifier->symbol->datatype;
}

enum DataType eval_func_exp(AstNode *identifier, AstNode *args_list) {
    TypeList *id_type = identifier->symbol->parameters;
    AstNode *arg = args_list;

    while ((id_type != NULL) && (arg != NULL)) {
        if (!is_compatible(id_type->type, check_nodes(arg->children[0]))) {
            print_type_error();
            semantic_errors++;
            break;
        }
    
        id_type = id_type->next;
        arg = arg->children[1];
    }

    if ((id_type == NULL) && (arg != NULL)) {
        print_arg_size_too_big_error();
        semantic_errors++;
    } else if ((id_type != NULL) && (arg == NULL)) {
        print_arg_size_too_small_error();
        semantic_errors++;
    }

    return identifier->symbol->datatype;
}

// Deixa AST_VEC_DECL avaliar tipos
enum DataType eval_lit_list(enum DataType t1, enum DataType t2) {
    enum DataType eval = DATATYPE_UNKNOWN;

    if ((is_char_or_int(t1) && is_char_or_int(t2))
            || (t1 == t2)) {
        eval = t1;
    }

    return eval;
}

enum DataType eval_var_attrib(
        enum DataType identifier_type,
        enum DataType expr_type) {
    if (!is_compatible(identifier_type, expr_type)) {
        print_type_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_vec_attrib(
        AstNode *identifier,
        AstNode *index,
        enum DataType expr_datatype) {
    enum DataType identifier_datatype = identifier->symbol->datatype;

    if (!is_compatible(identifier_datatype, expr_datatype)) {
        print_type_error();
        semantic_errors++;
    }

    if (!is_within_bounds(identifier, index)) {
        print_out_of_bounds_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_par(enum DataType type) {
    return type;
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

void print_type_error() {
    fprintf(stderr,
            "Semantic error: unexpected data type\n");
}

void print_arg_size_too_small_error() {
    fprintf(stderr,
            "Semantic error: too little arguments\n");
}

void print_arg_size_too_big_error() {
    fprintf(stderr,
            "Semantic error: too many arguments\n");
}

void print_out_of_bounds_error() {
    fprintf(stderr,
            "Semantic error: attempted to access out-of-bounds index\n");
}

void print_uncaught_parser_error() {
    fprintf(stderr, "Parser bug\n");
}

