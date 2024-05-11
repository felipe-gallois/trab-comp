// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef HASH_H
#define HASH_H

#define HASH_SIZE 997

enum SymbolType {
    SYMBOL_TK_IDENTIFIER,
    SYMBOL_LIT_INT,
    SYMBOL_LIT_CHAR,
    SYMBOL_LIT_REAL,
    SYMBOL_LIT_FALSE,
    SYMBOL_LIT_TRUE,
    SYMBOL_LIT_STRING,
    SYMBOL_VARIABLE,
    SYMBOL_VECTOR,
    SYMBOL_FUNCTION
};

enum DataType {
    DATATYPE_INT,
    DATATYPE_CHAR,
    DATATYPE_REAL,
    DATATYPE_BOOL
};

typedef struct HashEntry {
    int type;
    int datatype;
    char *string;
    struct HashEntry *next;
} HashEntry;

void hash_init();
int get_address(char *string);
HashEntry *find_entry(char *string);
HashEntry *insert_entry(char *string, int type);
void print_hash();

#endif
