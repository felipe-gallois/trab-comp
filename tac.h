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
    TAC_CALL,
    TAC_ARG,
    TAC_MOVE,
    TAC_VECMOVE,
    TAC_RET,
    TAC_PRINTINT,
    TAC_PRINTFLOAT,
    TAC_PRINTBOOL,
    TAC_PRINTCHAR,
    TAC_PRINTSTRING,
    TAC_READ,
    TAC_IFZ,
    TAC_LABEL,
    TAC_JUMP,
    TAC_BEGINFUN,
    TAC_ENDFUN,
};

typedef struct TacNode {
    enum TacType type;
    HashEntry *res;
    HashEntry *op1;
    HashEntry *op2;
    struct TacNode *neigh;
} TacNode;

TacNode *generate_code(AstNode *node);
void bind_parameters(AstNode *ast_root, TacNode *tac_list);
void bind_return_output(TacNode *tac_list);
TacNode *tac_reverse_list(TacNode *list);
void tac_print(TacNode *list);

#endif
