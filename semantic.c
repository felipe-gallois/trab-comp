// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "ast.h"
#include "semantic.h"

int semantic_errors = 0;

void check_return_types(AstNode *command, enum DataType func_type);
void check_lit_list_types(AstNode *lit_list, enum DataType vec_datatype);

enum DataType eval_symbol(HashEntry *symbol);
enum DataType eval_arith_op(enum DataType t1, enum DataType t2);
enum DataType eval_test_op(enum DataType t1, enum DataType t2);
enum DataType eval_bool_test_op(enum DataType t1, enum DataType t2);
enum DataType eval_not_op(enum DataType type);
enum DataType eval_vec_exp(AstNode *identifier, enum DataType index_datatype);
enum DataType eval_func_exp(AstNode *identifier, AstNode *args_list);
enum DataType eval_var_attrib(enum DataType identifier_type, enum DataType expr_type);
enum DataType eval_vec_attrib(enum DataType identifier_datatype, enum DataType index_datatype, enum DataType expr_datatype);
enum DataType eval_io_command(AstNode *type, enum DataType expr);
enum DataType eval_conditional_command(enum DataType conditional);
enum DataType eval_par(enum DataType type);
enum DataType eval_func_decl(enum DataType func_type, AstNode *command);
enum DataType eval_vec_decl(AstNode *size);
enum DataType eval_vec_decl_def(enum DataType vec_type, AstNode *size, AstNode *lit_list);
enum DataType eval_var_decl(enum DataType identifier_type, enum DataType expr_type);

void set_hash_type_from_decl_node(HashEntry *entry, AstNode *decl_node);
void set_hash_datatype_from_type_node(HashEntry *entry, AstNode *type_node);
void set_hash_capacity(HashEntry *identifier, HashEntry *capacity);
void set_hash_param_from_param_list_node(AstNode *param_list, TypeList **insert_place);

int is_declaration(AstNode *node);
int is_redeclared(HashEntry *entry);
int is_vector_decl(AstNode *node);
int is_func_decl(AstNode *node);
int is_char_or_int(enum DataType data_type);
int is_compatible(enum DataType t1, enum DataType t2);
int is_index(enum DataType data_type);

void print_redeclaration_error(char *identifier_name);
void print_undeclared_error(char *identifier_name);
void print_type_error();
void print_arg_size_too_small_error();
void print_arg_size_too_big_error();
void print_uncaught_parser_error();
void print_return_type_error();
void print_list_size_too_small_error();
void print_list_size_too_big_error();

enum DataType kw_to_datatype(AstNode *type);
enum DataType literal_to_datatype(AstNode *literal);
unsigned long get_index_from_literal(AstNode *index);
unsigned long get_list_size(AstNode *lit_list);

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
            node_eval = eval_vec_exp(node->children[0], children_eval[1]);
            break;
        case AST_FUNC_EXP:
            node_eval = eval_func_exp(node->children[0], node->children[1]);
            break;
        case AST_VAR_ATTRIB:
            node_eval = eval_var_attrib(children_eval[0], children_eval[1]);
            break;
        case AST_VEC_ATTRIB:
            node_eval = eval_vec_attrib(
                    children_eval[0],
                    children_eval[1],
                    children_eval[2]
            );
            break;
        case AST_PRINT_LIT:
        case AST_READ:
            node_eval = eval_io_command(node->children[0], children_eval[1]);
            break;
        case AST_IF:
        case AST_IF_ELSE:
        case AST_WHILE:
            node_eval = eval_conditional_command(children_eval[0]);
            break;
        case AST_PAR:
            node_eval = eval_par(children_eval[0]);
            break;
        case AST_FUNC_DECL:
            node_eval = eval_func_decl(children_eval[1], node->children[3]);
            break;
        case AST_VEC_DECL:
            node_eval = eval_vec_decl(node->children[2]);
            break;
        case AST_VEC_DECL_DEF:
            node_eval = eval_vec_decl_def(
                    children_eval[1],
                    node->children[2],
                    node->children[3]
            );
            break;
        case AST_VAR_DECL:
            node_eval = eval_var_decl(children_eval[1], children_eval[2]);
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

int is_index(enum DataType data_type) {
    return is_char_or_int(data_type);
}

unsigned long get_index_from_literal(AstNode *index) {
    if (index == NULL
            || index->type != AST_SYMBOL
            || index->symbol->type != SYMBOL_LIT_INT) {
        return -1;
    }

    return strtoul(index->symbol->string, NULL, 10);
}

unsigned long get_list_size(AstNode *lit_list) {
    if (lit_list == NULL)
        return 0;

    return get_list_size(lit_list->children[1]) + 1;
}

enum DataType kw_to_datatype(AstNode *type) {
    switch (type->type) {
        case AST_INT:
            return DATATYPE_INT;
        case AST_FLOAT:
            return DATATYPE_REAL;
        case AST_CHAR:
            return DATATYPE_CHAR;
        case AST_BOOL:
            return DATATYPE_BOOL;
        default:
            return DATATYPE_UNKNOWN;
    }
}

enum DataType literal_to_datatype(AstNode *literal) {
    switch (literal->symbol->type) {
        case SYMBOL_LIT_INT:
            return DATATYPE_INT;
        case SYMBOL_LIT_REAL:
            return DATATYPE_REAL;
        case SYMBOL_LIT_CHAR:
            return DATATYPE_CHAR;
        case SYMBOL_LIT_TRUE:
        case SYMBOL_LIT_FALSE:
            return DATATYPE_BOOL;
        default:
            return DATATYPE_UNKNOWN;
    }
}

void check_return_types(AstNode *command, enum DataType func_type) {
    if (command == NULL)
        return;

    if (command->type == AST_RET) {
        enum DataType return_type = check_nodes(command->children[0]);

        if (!is_compatible(return_type, func_type)) {
            print_return_type_error();
            semantic_errors++;
        }
    }

    for (int i = 0; i < MAX_CHILDREN; i++)
        check_return_types(command->children[i], func_type);
}

void check_lit_list_types(AstNode *lit_list, enum DataType vec_datatype) {
    if (lit_list == NULL)
        return;

    enum DataType lit_datatype = literal_to_datatype(lit_list->children[0]);

    if (!is_compatible(lit_datatype, vec_datatype)) {
        print_type_error();
        semantic_errors++;
    }

    check_lit_list_types(lit_list->children[1], vec_datatype);
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

enum DataType eval_vec_exp(
        AstNode *identifier,
        enum DataType index_datatype) {
    if (!is_index(index_datatype)) {
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
        enum DataType identifier_datatype,
        enum DataType index_datatype,
        enum DataType expr_datatype) {
    if (!is_compatible(identifier_datatype, expr_datatype)) {
        print_type_error();
        semantic_errors++;
    }

    if (!is_index(index_datatype)) {
        print_type_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_io_command(AstNode *type, enum DataType expr) {
    enum DataType kw_datatype = kw_to_datatype(type);

    if (!is_compatible(kw_datatype, expr)) {
        print_type_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_conditional_command(enum DataType conditional) {
    if (conditional != DATATYPE_BOOL) {
        print_type_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_par(enum DataType type) {
    return type;
}

enum DataType eval_func_decl(enum DataType func_type, AstNode *command) {
    check_return_types(command, func_type);
    return DATATYPE_UNKNOWN;
}

enum DataType eval_vec_decl(AstNode *size) {
    if (get_index_from_literal(size) == -1) {
        print_type_error();
        semantic_errors++;
    }
    return DATATYPE_UNKNOWN;
}

enum DataType eval_vec_decl_def(
        enum DataType vec_type,
        AstNode *size,
        AstNode *lit_list) {
    check_lit_list_types(lit_list, vec_type);

    unsigned long vec_size, list_size;

    if ((vec_size = get_index_from_literal(size)) == -1) {
        print_type_error();
        semantic_errors++;
    }

    list_size = get_list_size(lit_list);

    if (list_size < vec_size) {
        print_list_size_too_small_error();
        semantic_errors++;
    } else if (list_size > vec_size) {
        print_list_size_too_big_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
}

enum DataType eval_var_decl(enum DataType identifier_type, enum DataType expr_type) {
    if (!is_compatible(identifier_type, expr_type)) {
        print_type_error();
        semantic_errors++;
    }

    return DATATYPE_UNKNOWN;
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

void print_return_type_error() {
    fprintf(stderr,
            "Semantic error: returned type does not match function type\n");
}

void print_list_size_too_small_error() {
    fprintf(stderr,
            "Semantic error: literal list is too small\n");
}

void print_list_size_too_big_error() {
    fprintf(stderr,
            "Semantic error: literal list is too large\n");
}

void print_uncaught_parser_error() {
    fprintf(stderr, "Parser bug\n");
}

