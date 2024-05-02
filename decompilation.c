// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdio.h>

#include "hash.h"
#include "ast.h"

void decompile_tree(AstNode *node, FILE *out_file) {
    if (node == NULL)
        return;

    int type = node->type;

    switch (type) {
        case AST_SYMBOL:
            fprintf(out_file, "%s", node->symbol->string);
            break;
        case AST_BOOL:
            fprintf(out_file, "bool");
            break;
        case AST_FLOAT:
            fprintf(out_file, "float");
            break;
        case AST_INT:
            fprintf(out_file, "int");
            break;
        case AST_CHAR:
            fprintf(out_file, "char");
            break;
        case AST_SUM:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " + ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_SUB:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " - ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_MUL:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " * ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_DIV:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " / ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_LESS:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " < ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_GREATER:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " > ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_LE:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " <= ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_GE:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " >= ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_EQ:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " == ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_DIF:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " != ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_AND:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " & ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_OR:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " | ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_NOT:
            fprintf(out_file, "~");
            decompile_tree(node->children[0], out_file);
            break;
        case AST_VEC_EXP:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, "[");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, "]");
            break;
        case AST_FUNC_EXP:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, "(");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ")\n");
            break;
        case AST_ARGS_LIST:
            decompile_tree(node->children[0], out_file);
            if (node->children[1] != NULL) {
                fprintf(out_file, ", ");
                decompile_tree(node->children[1], out_file);
            }
            break;
        case AST_LIT_LIST:
            decompile_tree(node->children[0], out_file);
            if (node->children[1] != NULL) {
                fprintf(out_file, " ");
                decompile_tree(node->children[1], out_file);
            }
            break;
        case AST_CMD_LIST:
            decompile_tree(node->children[0], out_file);
            if (node->children[1] != NULL) {
                fprintf(out_file, "\n");
                decompile_tree(node->children[1], out_file);
            }
            break;
        case AST_VAR_ATTRIB:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " = ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, ";");
            break;
        case AST_VEC_ATTRIB:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, "[");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, "] = ");
            decompile_tree(node->children[2], out_file);
            fprintf(out_file, ";");
            break;
        case AST_RET:
            fprintf(out_file, "return ");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ";");
            break;
        case AST_EMPTY:
            fprintf(out_file, ";");
            break;
        case AST_PRINT_STRING:
            fprintf(out_file, "print ");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ";");
            break;
        case AST_PRINT_LIT:
            fprintf(out_file, "print ");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, ";");
            break;
        case AST_READ:
            fprintf(out_file, "read ");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, ";");
            break;
        case AST_IF:
            fprintf(out_file, "if (");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ") ");
            decompile_tree(node->children[1], out_file); 
            break;
        case AST_IF_ELSE:
            fprintf(out_file, "if (");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ") ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, " else ");
            decompile_tree(node->children[2], out_file);
            break;
        case AST_WHILE:
            fprintf(out_file, "while (");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ") ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_BLOCK:
            fprintf(out_file, "{");
            if (node->children[0] != NULL) {
                fprintf(out_file, "\n");
                decompile_tree(node->children[0], out_file);
                fprintf(out_file, "\n");
            }
            fprintf(out_file, "}");
            break;
        case AST_PAR:
            fprintf(out_file, "(");
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, ")");
            break;
        case AST_PARAM:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            break;
        case AST_PARAM_LIST:
            decompile_tree(node->children[0], out_file);
            if (node->children[1] != NULL) {
                fprintf(out_file, ", ");
                decompile_tree(node->children[1], out_file);
            }
            break;
        case AST_FUNC_DECL:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, "(");
            if (node->children[2] != NULL)
                decompile_tree(node->children[2], out_file);
            fprintf(out_file, ") ");
            decompile_tree(node->children[3], out_file);
            break;
        case AST_VEC_DECL:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, "[");
            decompile_tree(node->children[2], out_file);
            fprintf(out_file, "];");
            break;
        case AST_VEC_DECL_DEF:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, "[");
            decompile_tree(node->children[2], out_file);
            fprintf(out_file, "]:");
            decompile_tree(node->children[3], out_file);
            fprintf(out_file, ";");
            break;
        case AST_VAR_DECL:
            decompile_tree(node->children[0], out_file);
            fprintf(out_file, " ");
            decompile_tree(node->children[1], out_file);
            fprintf(out_file, ":");
            decompile_tree(node->children[2], out_file);
            fprintf(out_file, ";");
            break;
        case AST_DECL_LIST:
            decompile_tree(node->children[0], out_file);
            if (node->children[1] != NULL) {
                fprintf(out_file, "\n");
                decompile_tree(node->children[1], out_file);
            }
            break;
        default:
            break;
    }
}

int decompile(AstNode *root, char *output) {
    FILE *out_file;

    if ((out_file = fopen(output, "w")) == NULL)
        return -1;

    decompile_tree(root, out_file);

    fclose(out_file);

    return 0; 
}
