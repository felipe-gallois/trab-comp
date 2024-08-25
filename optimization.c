#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "optimization.h"

#define TABLE_SIZE 256

typedef struct VarListNode {
    enum TacType type;
    HashEntry *symbol;
    int op1_index;
    int op2_index;
} VarListNode;

typedef struct VarListDescr {
    int size;
    VarListNode *start;
} VarListDescr;

typedef struct VarTableNode {
    char *key; 
    int index;
    struct VarTableNode *next;
} VarTableNode;

int is_symbol(TacNode *tac) {
    if (tac->type == TAC_SYMBOL) {
        return 1;
    }

    return 0;
}

int is_unary_subexpr(TacNode *tac) {
    int is_unary_subexpr = 0;

    if (tac == NULL)
        return is_unary_subexpr;

    switch (tac->type) {
        case TAC_NOT:
            is_unary_subexpr = 1;
            break;
        default:
            break;
    }

    return is_unary_subexpr;
}

int is_binary_subexpr(TacNode *tac) {
    int is_binary_subexpr = 0;

    if (tac == NULL)
        return is_binary_subexpr;

    switch (tac->type) {
        case TAC_ADD:
            is_binary_subexpr = 1;
            break;
        default:
            break;
    }

    return is_binary_subexpr;
}

int is_subexpr(TacNode *tac) {
    if (is_symbol(tac) || is_unary_subexpr(tac) || is_binary_subexpr(tac)) {
        return 1;
    }
    
    return 0;
}

int count_subexprs(TacNode *tac_list) {
    int count = 0;

    while (tac_list != NULL) {
        if (is_subexpr(tac_list)) {
            count++;
        }

        tac_list = tac_list->neigh;
    }

    // DEBUG
    printf("Contei %d subexpressoes\n", count);

    return count;
}

VarListDescr *create_list(int size) {
    VarListNode *list = (VarListNode*) malloc(size * sizeof(VarListNode));

    VarListDescr *descr = (VarListDescr*) malloc(sizeof(VarListDescr));
    descr->size = 0;
    descr->start = list;

    // DEBUG
    printf("Criei lista\n");

    return descr;
}

int insert_list(VarListDescr *list, VarListNode data) {
    if (list == NULL)
        return -1;

    list->start[list->size] = data;
    list->size++;

    // DEBUG
    printf("Inseri na lista: %s\n", list->start[list->size - 1].symbol->string);

    return (list->size - 1);
}

void destroy_list(VarListDescr *list) {
    free(list->start);
    free(list);

    // DEBUG
    printf("Destrui lista\n");
}

VarTableNode **create_table(int size) {
    VarTableNode **table = (VarTableNode**) malloc(size * sizeof(VarTableNode*));

    for (int i = 0; i < size; i++) {
        table[i] = NULL;
    }

    // DEBUG
    printf("Criei tabela hash\n");

    return table;
}

int calculate_hash(char *key) {
    int address = 1;

    for (int i = 0; i < strlen(key); i++)
        address = (address * key[i]) % TABLE_SIZE + 1;

    return address - 1;
}

int search_index(VarTableNode **table, char *key) {
    int index = -1;
    int hash = calculate_hash(key); 

    VarTableNode *entry = table[hash];

    while (entry != NULL) {
        if (strcmp(entry->key, key) == 0) {
            index = entry->index;
            break;
        }

        entry = entry->next;
    }

    return index;
}

void insert_table(VarTableNode **table, char *key, int index) {
    if (table == NULL)
        return;

    VarTableNode *node = (VarTableNode*) malloc(sizeof(VarTableNode));
    node->key = key;
    node->index = index;
    node->next = NULL;

    int hash = calculate_hash(key);  
    VarTableNode *entry = table[hash];

    if (entry == NULL) {
        table[hash] = node;

        // DEBUG
        printf("Inseri na tabela hash: (key: %s, index: %d)\n", table[hash]->key, table[hash]->index);

        return;
    }

    while (entry->next != NULL) {
        entry = entry->next;
    }

    entry->next = node;

    // DEBUG
    printf("Inseri na tabela hash: (key: %s, index: %d)\n", entry->next->key, entry->next->index);
}

void destroy_table_node_recursive(VarTableNode *table_node) {
    if (table_node != NULL) {
        destroy_table_node_recursive(table_node->next);
        free(table_node);
    }
}

void destroy_table(VarTableNode **table, int size) {
    for (int i = 0; i < size; i++) {
        destroy_table_node_recursive(table[i]); 
    }

    // DEBUG
    printf("Destrui tabela hash\n");
}

int search_operation(VarListDescr *list, enum TacType type, int op1_index, int op2_index) {
    VarListNode node;

    for (int i = 0; i < list->size; i++) {
        node = list->start[i];

        if ((node.type == type)
                && (node.op1_index == op1_index)
                && (node.op2_index == op2_index)) {

            printf("Operacao repetida\n");
            return i;
        }
    }

    return -1;
}

void process_symbol(VarListDescr *list, VarTableNode **table, TacNode *tac) {
    int index = search_index(table, tac->res->string);

    if (index < 0) {
        VarListNode list_node = { tac->type, tac->res, -1, -1 };
        index = insert_list(list, list_node);
        insert_table(table, tac->res->string, index);
    }
}

void process_unary_subexpr(VarListDescr *list, VarTableNode **table, TacNode *tac) {
    int op_index = search_index(table, tac->op1->string);

    if (op_index < 0) {
        printf("Erro: esperava indice\n");
        return;
    }

    int res_index = search_operation(list, tac->type, op_index, -1);

    if (res_index < 0) {
        VarListNode list_node = { tac->type, tac->res, op_index, -1 };
        res_index = insert_list(list, list_node);
    }

    insert_table(table, tac->res->string, res_index);
}

void process_binary_subexpr(VarListDescr *list, VarTableNode **table, TacNode *tac) {
    int op1_index = search_index(table, tac->op1->string);
    int op2_index = search_index(table, tac->op2->string);

    if (op1_index < 0 || op2_index < 0) {
        printf("Erro: esperava indice\n");
        return;
    }

    int res_index = search_operation(list, tac->type, op1_index, op2_index);

    if (res_index < 0) {
        VarListNode list_node = { tac->type, tac->res, op1_index, op2_index };
        res_index = insert_list(list, list_node);
    }

    insert_table(table, tac->res->string, res_index);
}

int is_alias(VarListDescr *list, VarTableNode **table, char *name) {
    int index = search_index(table, name); 

    VarListNode node = list->start[index];

    if (strcmp(node.symbol->string, name) != 0) {
        return 1;
    }

    return 0;
}

void optimize_common_subexpr(TacNode *tac_list) {
    TacNode *tac_node;
    TacNode *prev_node;

    int num_subexprs = count_subexprs(tac_list);
    VarListDescr *vars_list = create_list(num_subexprs);     

    VarTableNode **var_table = create_table(TABLE_SIZE);

    tac_node = tac_list;

    while (tac_node != NULL) {
        if (is_symbol(tac_node)) {
            process_symbol(vars_list, var_table, tac_node);
        } else if (is_unary_subexpr(tac_node)) {
            process_unary_subexpr(vars_list, var_table, tac_node);
        } else if (is_binary_subexpr(tac_node)) {
            process_binary_subexpr(vars_list, var_table, tac_node);
        }

        tac_node = tac_node->neigh;
    }

    tac_node = tac_list;

    while (tac_node != NULL) {
        int index;
        prev_node = tac_node;
        tac_node = tac_node->neigh;

        if (is_subexpr(prev_node)) {
            if (is_alias(vars_list, var_table, prev_node->res->string)) {
                delete_temp(prev_node->res);
                delete_tac(tac_list, prev_node);  
                continue;
            }

            if (prev_node->op1 != NULL) {
                index = search_index(var_table, prev_node->op1->string);
                if (index >= 0) {
                    prev_node->op1 = vars_list->start[index].symbol;
                }
            }

            if (prev_node->op2 != NULL) {
                index = search_index(var_table, prev_node->op2->string);
                if (index >= 0) {
                    prev_node->op2 = vars_list->start[index].symbol;
                }
            }

            if (prev_node->res != NULL) {
                index = search_index(var_table, prev_node->res->string);
                if (index >= 0) {
                    prev_node->res = vars_list->start[index].symbol;
                }
            }
        }
    }

    destroy_list(vars_list);
    destroy_table(var_table, TABLE_SIZE);
}
