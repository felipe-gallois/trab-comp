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
        case AST_VAR_EXP:
            fprintf(stderr, "AST_VAR_EXP");
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
        case AST_LIT_LIST:
            fprintf(stderr, "AST_LIT_LIST");
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
        case AST_PRINT_STRING:
            fprintf(stderr, "AST_PRINT_STRING");
            break;
        case AST_PRINT_LIT:
            fprintf(stderr, "AST_PRINT_LIT");
            break;
        case AST_READ:
            fprintf(stderr, "AST_READ");
            break;
        case AST_IF:
            fprintf(stderr, "AST_IF");
            break;
        case AST_IF_ELSE:
            fprintf(stderr, "AST_IF_ELSE");
            break;
        case AST_WHILE:
            fprintf(stderr, "AST_WHILE");
            break;
        case AST_BLOCK:
            fprintf(stderr, "AST_BLOCK");
            break;
        case AST_PAR:
            fprintf(stderr, "AST_PAR");
            break;
        case AST_PARAM:
            fprintf(stderr, "AST_PARAM");
            break;
        case AST_PARAM_LIST:
            fprintf(stderr, "AST_PARAM_LIST");
            break;
        case AST_FUNC_DECL:
            fprintf(stderr, "AST_FUNC_DECL");
            break;
        case AST_VEC_DECL:
            fprintf(stderr, "AST_VEC_DECL");
            break;
        case AST_VEC_DECL_DEF:
            fprintf(stderr, "AST_VEC_DECL_DEF");
            break;
        case AST_VAR_DECL:
            fprintf(stderr, "AST_VAR_DECL");
            break;
        case AST_DECL_LIST:
            fprintf(stderr, "AST_DECL_LIST");
            break;
        default:
            fprintf(stderr, "AST_UNKNOWN");
    }

    if (node->symbol != NULL) {
        fprintf(stderr, ",");
        fprintf(stderr, "%s", node->symbol->string);
    }

    fprintf(stderr, ")\n");
}

void ast_print(AstNode *node, int level) {
    if (node == NULL)
        return;

    ast_print_node(node, level);

    for (int i = 0; i < MAX_CHILDREN; i++)
        ast_print(node->children[i], level + 1);
}

void write_var_decl(FILE *asm_file, AstNode *tree) {
    AstNode *declaration;

    while (tree != NULL) {
        declaration = tree->children[0];

        if (declaration->type == AST_VAR_DECL) {
            fprintf(asm_file, "_%s:\n", declaration->children[1]->symbol->string);
            switch (declaration->children[1]->symbol->datatype) {
                case DATATYPE_INT:
                    fprintf(asm_file, "\t.long\t%s\n", declaration->children[2]->symbol->string);
                    break;
                case DATATYPE_REAL:
                    fprintf(asm_file, "\t.float\t%s\n", declaration->children[2]->symbol->string);
                    break;
                case DATATYPE_BOOL:
                    if (declaration->children[2]->symbol->type == SYMBOL_LIT_FALSE) {
                        fprintf(asm_file, "\t.long\t0\n");
                    } else {
                        fprintf(asm_file, "\t.long\t1\n");
                    }
                    break;
                case DATATYPE_CHAR:
                    fprintf(asm_file, "\t.byte\t%s\n", declaration->children[2]->symbol->string);
                    break;
                default:
                    break;
            }
        }

        tree = tree->children[1];
    }
}

void write_vec_decl(FILE *asm_file, AstNode *tree) {
    AstNode *declaration;

    while (tree != NULL) {
        declaration = tree->children[0];

        if (declaration->type == AST_VEC_DECL) {
            fprintf(asm_file, "_%s:\n"
                    "\t.zero\t",
                    declaration->children[1]->symbol->string
            );
            switch (declaration->children[1]->symbol->datatype) {
                case DATATYPE_INT:
                case DATATYPE_REAL:
                case DATATYPE_BOOL:
                    fprintf(asm_file,
                            "%d\n",
                            4 * atoi(declaration->children[2]->symbol->string));
                    break;
                case DATATYPE_CHAR:
                    fprintf(asm_file,
                            "%d\n",
                            atoi(declaration->children[2]->symbol->string));
                    break;
                default:
                    break;
            }
        }

        tree = tree->children[1];
    }
}

void write_literal(FILE *asm_file, AstNode *literal) {
    if (literal->symbol->type == SYMBOL_LIT_TRUE) {
        fprintf(asm_file, "1");
    } else if (literal->symbol->type == SYMBOL_LIT_FALSE) {
        fprintf(asm_file, "0");
    } else {
        fprintf(asm_file, "%s", literal->symbol->string);
    }
}

void write_literal_list(FILE *asm_file, AstNode *lit_list) {
    write_literal(asm_file, lit_list->children[0]);

    for (lit_list = lit_list->children[1];
            lit_list != NULL;
            lit_list = lit_list->children[1]) {
        fprintf(asm_file, ", ");
        write_literal(asm_file, lit_list->children[0]); 
    }

    fprintf(asm_file, "\n");
}

void write_vec_decl_def(FILE *asm_file, AstNode *tree) {
    AstNode *declaration;

    while (tree != NULL) {
        declaration = tree->children[0];

        if (declaration->type == AST_VEC_DECL_DEF) {
            fprintf(asm_file, "_%s:\n", declaration->children[1]->symbol->string);
            switch (declaration->children[1]->symbol->datatype) {
                case DATATYPE_INT:
                    fprintf(asm_file, "\t.long\t");
                    break;
                case DATATYPE_REAL:
                    fprintf(asm_file, "\t.float\t");
                    break;
                case DATATYPE_BOOL:
                    if (declaration->children[2]->symbol->type == SYMBOL_LIT_FALSE) {
                        fprintf(asm_file, "\t.long\t");
                    } else {
                        fprintf(asm_file, "\t.long\t");
                    }
                    break;
                case DATATYPE_CHAR:
                    fprintf(asm_file, "\t.byte\t");
                    break;
                default:
                    break;
            }
            write_literal_list(asm_file, declaration->children[3]);
        }

        tree = tree->children[1];
    }
}

void write_parameters(FILE *asm_file, AstNode *tree) {
    AstNode *declaration;
    AstNode *param_list;
    HashEntry *param_symbol;

    while (tree != NULL) {
        declaration = tree->children[0];

        if (declaration->type == AST_FUNC_DECL) {
            param_list = declaration->children[2]; 

            while (param_list != NULL) {
                param_symbol = param_list->children[0]->children[1]->symbol;
                
                fprintf(asm_file, "_%s:\n"
                        "\t.zero\t",
                        param_symbol->string
                );
                switch (param_symbol->datatype) {
                    case DATATYPE_INT:
                    case DATATYPE_REAL:
                    case DATATYPE_BOOL:
                        fprintf(asm_file, "4\n");
                        break;
                    case DATATYPE_CHAR:
                        fprintf(asm_file, "1\n");
                        break;
                    default:
                        break;
                }

                param_list = param_list->children[1];
            }
        }

        tree = tree->children[1];
    }
}

