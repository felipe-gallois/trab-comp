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

HashEntry *insert_entry(char *string, enum SymbolType type, enum DataType datatype) {
    int address = get_address(string);

    HashEntry *new_entry;
    if ((new_entry = find_entry(string)) != 0)
        return new_entry;

    new_entry = (HashEntry*) malloc(sizeof(HashEntry));
    new_entry->type = type;
    new_entry->datatype = datatype;
    new_entry->capacity = 0;
    new_entry->parameters = NULL;
    new_entry->string = (char*) malloc((strlen(string) + 1)
                                       * sizeof(HashEntry));
    strcpy(new_entry->string, string);
    new_entry->next = NULL;
    table[address] = new_entry;
    return new_entry;
}

HashEntry *makeTemp(enum DataType datatype) {
    static int serial = 0;

    HashEntry *result;
    char buffer[256] = "";

    sprintf(buffer, "_temp%d", serial);

    result = insert_entry(buffer, SYMBOL_TEMP, datatype);
    serial++;

    return result;
}

HashEntry *makeLabel() {
    static int serial = 0;

    HashEntry *result;
    char buffer[256] = "";

    sprintf(buffer, "_label%d", serial);

    result = insert_entry(buffer, SYMBOL_LABEL, DATATYPE_UNKNOWN);
    serial++;

    return result;
}

void enumerate_literals() {
    int int_counter = 0;
    int char_counter = 0;
    int float_counter = 0;
    int string_counter = 0;

    for (int i = 0; i < HASH_SIZE; i++) {
        for (HashEntry *entry = table[i]; entry; entry = entry->next) {
            switch (entry->type) {
                case SYMBOL_LIT_INT:
                    sprintf(entry->id, "int%d", int_counter);
                    int_counter++;
                    break;
                case SYMBOL_LIT_CHAR:
                    sprintf(entry->id, "char%d", char_counter);
                    char_counter++;
                    break;
                case SYMBOL_LIT_REAL:
                    sprintf(entry->id, "float%d", float_counter);
                    float_counter++;
                    break;
                case SYMBOL_LIT_FALSE:
                    sprintf(entry->id, "false");
                    break;
                case SYMBOL_LIT_TRUE:
                    sprintf(entry->id, "true");
                    break;
                case SYMBOL_LIT_STRING:
                    sprintf(entry->id, "string%d", string_counter);
                    string_counter++;
                    break;
                default:
                    break;
            }
        }
    }
}

void write_literals(FILE *asm_file) {
    for (int i = 0; i < HASH_SIZE; i++) {
        for (HashEntry *entry = table[i]; entry; entry = entry->next) {
            switch (entry->type) {
                case SYMBOL_LIT_INT:
                    fprintf(asm_file, "_%s:\n\t.long\t%s\n", entry->id, entry->string); 
                    break;
                case SYMBOL_LIT_REAL:
                    fprintf(asm_file, "_%s:\n\t.float\t%s\n", entry->id, entry->string); 
                    break;
                case SYMBOL_LIT_FALSE:
                    fprintf(asm_file, "_%s:\n\t.long\t0\n", entry->id); 
                    break;
                case SYMBOL_LIT_TRUE:
                    fprintf(asm_file, "_%s:\n\t.long\t1\n", entry->id); 
                    break;
                case SYMBOL_LIT_CHAR:
                    fprintf(asm_file, "_%s:\n\t.byte\t%s\n", entry->id, entry->string); 
                    break;
                case SYMBOL_LIT_STRING:
                    fprintf(asm_file, "_%s:\n\t.string\t%s\n", entry->id, entry->string); 
                    break;
                default:
                    break;
            }
        }
    }
}

void write_temps(FILE *asm_file) {
    int size;

    for (int i = 0; i < HASH_SIZE; i++) {
        for (HashEntry *entry = table[i]; entry; entry = entry->next) {
            if (entry->type == SYMBOL_TEMP) {
                fprintf(asm_file, "_%s:\n"
                        "\t.zero\t",
                        entry->string
                );
                switch (entry->datatype) {
                    case DATATYPE_INT:
                    case DATATYPE_REAL:
                    case DATATYPE_BOOL:
                        size = 4;
                        break;
                    case DATATYPE_CHAR:
                        size = 1;
                        break;
                    default:
                        size = 0;
                        break;
                }

                fprintf(asm_file,
                        "%d\n",
                        size);
            }
        }
    }
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

