// UFRGS - Compiladores - Felipe Gallois - 2024/1

#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "hash.h"

static HashEntry *table[HASH_SIZE];

void hash_init() {
    for (int i = 0; i < HASH_SIZE; i++) {
        table[i] = NULL;
    }
}

int get_address(char *string) {
    int address = 1;
    for (int i = 0; i < strlen(string); i++)
        address = (address * string[i]) % HASH_SIZE + 1;
    return address - 1;
}

HashEntry *find_entry(char *string) {
    int address = get_address(string);
    for (HashEntry *entry = table[address]; entry; entry = entry->next) {
        if (strcmp(entry->string, string) == 0)
            return entry;
    }
    return NULL;    // no matches
}

HashEntry *insert_entry(char *string, enum SymbolType type) {
    int address = get_address(string);

    HashEntry *new_entry;
    if ((new_entry = find_entry(string)) != 0)
        return new_entry;

    new_entry = (HashEntry*) malloc(sizeof(HashEntry));
    new_entry->type = type;
    new_entry->datatype = DATATYPE_UNKNOWN;
    new_entry->capacity = 0;
    new_entry->parameters = NULL;
    new_entry->string = (char*) malloc((strlen(string) + 1)
                                       * sizeof(HashEntry));
    strcpy(new_entry->string, string);
    new_entry->next = NULL;
    table[address] = new_entry;
    return new_entry;
}

HashEntry *makeTemp() {
    static int serial = 0;

    HashEntry *result;
    char buffer[256] = "";

    sprintf(buffer, "_temp%d", serial);

    result = insert_entry(buffer, SYMBOL_TEMP);
    serial++;

    return result;
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

