// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef SYMBOLS_TABLE_H
#define SYMBOLS_TABLE_H

#include "symbol.h"

typedef void* SymbolsTableHandle;

int hash_init(SymbolsTableHandle *handle);
int find_symbol(SymbolsTableHandle handle, symbol_t *output, char *string);
int insert_symbol(SymbolsTableHandle handle, symbol_t symbol);
int count_undeclared(SymbolsTableHandle handle);
void print_hash();

#endif
