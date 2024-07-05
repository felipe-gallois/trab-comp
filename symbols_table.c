#include <stdlib.h>
#include <string.h>

#include "symbols_table.h"

#define HASH_SIZE 997

typedef struct HashEntry {
    symbol_t symbol;
    struct HashEntry *next;
} hash_entry_t;

int get_hash(char *string);

int hash_init(SymbolsTableHandle *handle) {
    hash_entry_t **ptr;
    hash_entry_t **it;

    ptr = (hash_entry_t**) malloc(HASH_SIZE * sizeof(hash_entry_t*));
    it = ptr;
    for (int i = 0; i < HASH_SIZE; i++) {
        *it = NULL;
        it += 1;
    }

    *handle = (SymbolsTableHandle) ptr;
    return 0; 
}

int find_symbol(SymbolsTableHandle handle, symbol_t* output, char *string) {
    if (handle == NULL)
        return -1;

    int address = get_hash(string);

    hash_entry_t *list_pos = *((hash_entry_t **) handle);
    list_pos += address;

    for (; list_pos != NULL; list_pos = list_pos->next) {
        if (strcmp(list_pos->symbol.string, string) == 0) {
            *output = list_pos->symbol;
            return 0;
        }
    }

    return -1;    // no matches
}

int insert_symbol(SymbolsTableHandle handle, symbol_t symbol) {
    if (handle == NULL)
        return -1;

    int address = get_hash(symbol.string);

    hash_entry_t *list_pos = *((hash_entry_t **) handle);
    list_pos += address;

    hash_entry_t *new_entry = (hash_entry_t*) malloc(sizeof(hash_entry_t));
    new_entry->symbol = symbol;
    new_entry->next = NULL;

    for (; list_pos != NULL; list_pos = list_pos->next);

    list_pos = new_entry;

    return 0;
}

int count_undeclared(SymbolsTableHandle handle) {
    if (handle == NULL)
        return -1;

    int count = 0;

    hash_entry_t *list_pos = *((hash_entry_t **) handle);

    for (int i = 0; i < HASH_SIZE; i++, list_pos++) {
        if (li

    //    for (HashEntry *entry = table[i]; entry; entry = entry->next) {
    //        if (entry->type == SYMBOL_TK_IDENTIFIER) {
    //            fprintf(stderr,
    //                    "Semantic error: identifier %s undeclared\n",
    //                    entry->string); 
    //            count++;
    //        }
    //    }
    }

    return count;
}

void print_hash() {
    for (int i = 0; i < HASH_SIZE; i++) {
        for (HashEntry *entry = table[i]; entry; entry = entry->next)
            printf("Table[%d] has %s with type %d and datatype %d\n",
                    i,
                    entry->string,
                    entry->type,
                    entry->datatype);
    }
}

int get_hash(char *string) {
    int address = 1;
    for (int i = 0; i < strlen(string); i++)
        address = (address * string[i]) % HASH_SIZE + 1;
    return address - 1;
}

