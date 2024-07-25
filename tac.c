// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stdlib.h>
#include <stdio.h>

#include "tac.h"

static char *tac_strings[] = {
    "TAC_SYMBOL",
};

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
        default:
            result = generate_default(children_code);
            break;
    }

    return result;
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
        res = list->res;
        op1 = list->op1;
        op2 = list->op2;

        printf("TAC(%s, ", tac_strings[list->type]);
        printf("%s, ", res == NULL ? "0" : res->string); 
        printf("%s, ", op1 == NULL ? "0" : op1->string); 
        printf("%s)\n", op2 == NULL ? "0" : op2->string); 

        list = list->neigh;
    }
}

