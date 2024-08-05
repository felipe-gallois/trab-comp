#include <stdio.h>

#include "hash.h"
#include "asm.h"

void write_read_only(FILE *asm_file) {
    fprintf(asm_file, "## READ-ONLY\n"
            ".section\t.rodata\n");
    write_literals(asm_file);
}

void generateAsm(const char *file_name) {
    FILE *file = fopen(file_name, "w");    
    write_read_only(file);
    fclose(file);
}
