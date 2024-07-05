// UFRGS - Compiladores - Felipe Gallois - 2024/1

#ifndef SYMBOL_H
#define SYMBOL_H

typedef enum SymbolType {
    SYMBOL_TK_IDENTIFIER,
    SYMBOL_LIT_INT,
    SYMBOL_LIT_CHAR,
    SYMBOL_LIT_REAL,
    SYMBOL_LIT_FALSE,
    SYMBOL_LIT_TRUE,
    SYMBOL_LIT_STRING,
    SYMBOL_VARIABLE,
    SYMBOL_VECTOR,
    SYMBOL_FUNCTION,
} symbol_type_t;

typedef enum DataType {
    DATATYPE_UNKNOWN,
    DATATYPE_INT,
    DATATYPE_CHAR,
    DATATYPE_REAL,
    DATATYPE_BOOL,
    DATATYPE_STRING,
} data_type_t;

typedef struct Symbol {
    symbol_type_t type;
    data_type_t datatype;
    char *string;
} symbol_t;

#endif
