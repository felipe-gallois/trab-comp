// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "hash.h"
#include "tac.h"

static char *tac_strings[] = {
    "TAC_SYMBOL",
    "TAC_ADD",
    "TAC_SUB",
    "TAC_MUL",
    "TAC_DIV",
    "TAC_LESS",
    "TAC_GREATER",
    "TAC_LE",
    "TAC_GE",
    "TAC_EQ",
    "TAC_DIF",
    "TAC_AND",
    "TAC_OR",
    "TAC_NOT",
    "TAC_VECREAD",
    "TAC_MOVE",
    "TAC_VECMOVE",
    "TAC_RET",
    "TAC_PRINT",
    "TAC_READ",
    "TAC_IFZ",
    "TAC_LABEL",
    "TAC_JUMP",
};

TacNode *generate_binary_op(enum TacType type, TacNode *children_code[]);
TacNode *generate_unary_op(enum TacType type, TacNode *children_code[]);
TacNode *generate_vec_exp(TacNode *children_code[]);
TacNode *generate_move(TacNode *children_code[]);
TacNode *generate_vec_attrib(TacNode *children_code[]);
TacNode *generate_return(TacNode *children_code[]);
TacNode *generate_print_string(TacNode *children_code[]);
TacNode *generate_print_lit(TacNode *children_code[]);
TacNode *generate_read(TacNode *children_code[]);
TacNode *generate_if(TacNode *children_code[]);
TacNode *generate_if_else(TacNode *children_code[]);
TacNode *generate_while(TacNode *children_code[]);
TacNode *generate_default(TacNode *children_code[]);

TacNode *tac_create(enum TacType type, HashEntry *res, HashEntry *op1,
                    HashEntry *op2);
TacNode *tac_join(TacNode *list1, TacNode *list_2);

TacNode *generate_code(AstNode *node) {
    TacNode *children_code[MAX_CHILDREN];
    TacNode *result = NULL;

    if (node == NULL)
        return result;

    for (int i = 0; i < MAX_CHILDREN; i++)
        children_code[i] = generate_code(node->children[i]);

    switch (node->type) {
        case AST_SYMBOL:
            result = tac_create(TAC_SYMBOL, node->symbol, NULL, NULL);
            break;
        case AST_SUM:
            result = generate_binary_op(TAC_ADD, children_code);
            break;
        case AST_SUB:
            result = generate_binary_op(TAC_SUB, children_code);
            break;
        case AST_MUL:
            result = generate_binary_op(TAC_MUL, children_code);
            break;
        case AST_DIV:
            result = generate_binary_op(TAC_DIV, children_code);
            break;
        case AST_LESS:
            result = generate_binary_op(TAC_LESS, children_code);
            break;
        case AST_GREATER:
            result = generate_binary_op(TAC_GREATER, children_code);
            break;
        case AST_LE:
            result = generate_binary_op(TAC_LE, children_code);
            break;
        case AST_GE:
            result = generate_binary_op(TAC_GE, children_code);
            break;
        case AST_EQ:
            result = generate_binary_op(TAC_EQ, children_code);
            break;
        case AST_DIF:
            result = generate_binary_op(TAC_DIF, children_code);
            break;
        case AST_AND:
            result = generate_binary_op(TAC_AND, children_code);
            break;
        case AST_OR:
            result = generate_binary_op(TAC_OR, children_code);
            break;
        case AST_NOT:
            result = generate_unary_op(TAC_NOT, children_code);
            break;
        // TODO: AST_VEC_EXP .. AST_CMD_LIST
        case AST_VEC_EXP:
            result = generate_vec_exp(children_code);
            break;
        case AST_VAR_ATTRIB:
            result = generate_move(children_code);
            break;
        case AST_VEC_ATTRIB:
            result = generate_vec_attrib(children_code);
            break;
        case AST_RET:
            result = generate_return(children_code);
            break;
        case AST_PRINT_STRING:
            result = generate_print_string(children_code);
            break;
        case AST_PRINT_LIT:
            result = generate_print_lit(children_code);
            break;
        case AST_READ:
            result = generate_read(children_code);
            break;
        case AST_IF:
            result = generate_if(children_code);
            break;
        case AST_IF_ELSE:
            result = generate_if_else(children_code);
            break;
        case AST_WHILE:
            result = generate_while(children_code);
            break;
        // TODO: AST_PARAM_LIST .. AST_VAR_DECL
        default:
            result = generate_default(children_code);
            break;
    }

    return result;
}

TacNode *generate_binary_op(enum TacType type, TacNode *children_code[]) {
    TacNode *result = tac_create(
            type,
            makeTemp(),
            children_code[0] ? children_code[0]->res : NULL,
            children_code[1] ? children_code[1]->res : NULL
    );

    return tac_join(children_code[0], tac_join(children_code[1], result));
}

TacNode *generate_unary_op(enum TacType type, TacNode *children_code[]) {
    TacNode *result = tac_create(
            type,
            makeTemp(),
            children_code[0] ? children_code[0]->res : NULL,
            NULL
    );

    return tac_join(children_code[0], result);
}

TacNode *generate_vec_exp(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_VECREAD,
            makeTemp(),
            children_code[0] ? children_code[0]->res : NULL,
            children_code[1] ? children_code[1]->res : NULL
    );

    return tac_join(children_code[1], result);
}

TacNode *generate_move(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_MOVE,
            children_code[0] ? children_code[0]->res : NULL,
            children_code[1] ? children_code[1]->res : NULL,
            NULL
    );

    return tac_join(children_code[0], tac_join(children_code[1], result));
}

TacNode *generate_vec_attrib(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_VECMOVE,
            children_code[0] ? children_code[0]->res : NULL,
            children_code[1] ? children_code[1]->res : NULL,
            children_code[2] ? children_code[2]->res : NULL
    );

    return tac_join(
            children_code[1],
            tac_join(
                children_code[2],
                result
            )
    );
}

TacNode *generate_return(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_RET,
            children_code[0] ? children_code[0]->res : NULL,
            NULL,
            NULL
    );

    return tac_join(children_code[0], result);
}

TacNode *generate_print_string(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_PRINT,
            NULL,
            children_code[0] ? children_code[0]->res : NULL,
            NULL
    );

    return tac_join(children_code[0], result);
}

TacNode *generate_print_lit(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_PRINT,
            NULL,
            children_code[1] ? children_code[1]->res : NULL,
            NULL
    );

    return tac_join(children_code[1], result);
}

TacNode *generate_read(TacNode *children_code[]) {
    TacNode *result = tac_create(
            TAC_READ,
            children_code[1] ? children_code[1]->res : NULL,
            NULL,
            NULL
    );

    return tac_join(children_code[1], result);
}

TacNode *generate_if(TacNode *children_code[]) {
    TacNode *label = tac_create(
            TAC_LABEL,
            makeLabel(),
            NULL,
            NULL
    );

    TacNode *test = tac_create(
            TAC_IFZ,
            label->res,
            children_code[0] ? children_code[0]->res : NULL,
            NULL
    );

    return tac_join(
            children_code[0],
            tac_join(test,
                tac_join(children_code[1],
                    label)));
}

TacNode *generate_if_else(TacNode *children_code[]) {
    TacNode *else_label = tac_create(
            TAC_LABEL,
            makeLabel(),
            NULL,
            NULL
    );

    TacNode *continue_label = tac_create(
            TAC_LABEL,
            makeLabel(),
            NULL,
            NULL
    );

    TacNode *test = tac_create(
            TAC_IFZ,
            else_label->res,
            children_code[0] ? children_code[0]->res : NULL,
            NULL
    );

    TacNode *continue_jump = tac_create(
            TAC_JUMP,
            continue_label->res,
            NULL,
            NULL
    );

    return tac_join(
            children_code[0],
            tac_join(
                test,
                tac_join(
                    children_code[1],
                    tac_join(
                        continue_jump,
                        tac_join(
                            else_label,
                            tac_join(
                                children_code[2],
                                continue_label
                            )
                        )
                    )
                )
            )
    );
}

TacNode *generate_while(TacNode *children_code[]) {
    TacNode *test_label = tac_create(
            TAC_LABEL,
            makeLabel(),
            NULL,
            NULL
    );

    TacNode *continue_label = tac_create(
            TAC_LABEL,
            makeLabel(),
            NULL,
            NULL
    );

    TacNode *test = tac_create(
            TAC_IFZ,
            continue_label->res,
            children_code[0] ? children_code[0]->res : NULL,
            NULL
    );

    TacNode *test_jump = tac_create(
            TAC_JUMP,
            test_label->res,
            NULL,
            NULL
    );

    return tac_join(
            children_code[0],
            tac_join(
                test_label,
                tac_join(
                    test,
                    tac_join(
                        children_code[1],
                        tac_join(
                            test_jump,
                            continue_label
                        )
                    )
                )
            )
    );
}

TacNode *generate_default(TacNode *children_code[]) {
    TacNode *result = children_code[MAX_CHILDREN-1];

    for (int i = MAX_CHILDREN - 2; i >= 0; i--)
        result = tac_join(children_code[i], result);

    return result;
}

TacNode *tac_create(enum TacType type, HashEntry *res, HashEntry *op1,
        HashEntry *op2) {
    TacNode *new_node = (TacNode*) malloc(sizeof(TacNode));

    new_node->type = type;
    new_node->res = res;
    new_node->op1 = op1;
    new_node->op2 = op2;
    new_node->neigh = NULL;

    return new_node;
}

TacNode *tac_join(TacNode *list1, TacNode *list2) {
    TacNode *list2_start;

    if (list1 == NULL)
        return list2;

    if (list2 == NULL)
        return list1;

    // Atinge o início de list2
    for (list2_start = list2;
            list2_start->neigh != NULL;
            list2_start = list2_start->neigh);

    list2_start->neigh = list1;

    return list2;
}

TacNode *tac_reverse_list(TacNode *list) {
    TacNode *prev_neigh, *next_neigh;

    prev_neigh = NULL;

    while (list != NULL) {
        next_neigh = list->neigh;

        list->neigh = prev_neigh;

        prev_neigh = list;
        list = next_neigh;
    }

    return prev_neigh;
}

void tac_print(TacNode *list) {
    HashEntry *res, *op1, *op2;

    while (list != NULL) {
        // Não imprime TAC_SYMBOL
        if (list->type != TAC_SYMBOL) {
            res = list->res;
            op1 = list->op1;
            op2 = list->op2;

            printf("TAC(%s, ", tac_strings[list->type]);
            printf("%s, ", res == NULL ? "NULL" : res->string); 
            printf("%s, ", op1 == NULL ? "NULL" : op1->string); 
            printf("%s)\n", op2 == NULL ? "NULL" : op2->string); 
        }

        list = list->neigh;
    }
}

