// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef TAC_H
#define TAC_H

#include "hash.h"
#include "ast.h"

enum TacType {
    TAC_SYMBOL,
    TAC_ADD,
    TAC_SUB,
    TAC_MUL,
    TAC_DIV,
    TAC_LESS,
    TAC_GREATER,
    TAC_LE,
    TAC_GE,
    TAC_EQ,
    TAC_DIF,
    TAC_AND,
    TAC_OR,
    TAC_NOT,
    TAC_VECREAD,
    TAC_MOVE,
    TAC_VECMOVE,
    TAC_RET,
    TAC_PRINT,
    TAC_READ,
    TAC_IFZ,
    TAC_LABEL,
    TAC_JUMP,
};

typedef struct TacNode {
    enum TacType type;
    HashEntry *res;
    HashEntry *op1;
    HashEntry *op2;
    struct TacNode *neigh;
} TacNode;

TacNode *generate_code(AstNode *node);
TacNode *tac_reverse_list(TacNode *list);
void tac_print(TacNode *list);

#endif
