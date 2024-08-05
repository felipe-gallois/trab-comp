#include <stdio.h>

#include "hash.h"
#include "asm.h"

void write_read_only(FILE *asm_file) {
    fprintf(asm_file, "## READ-ONLY\n"
            ".section\t.rodata\n");
    write_literals(asm_file);
}

void write_instructions(FILE *asm_file, TacNode *tac_list) {
    fprintf(asm_file, "## INSTRUCTIONS\n"
            ".text\n");
    
    while (tac_list != NULL) {
        switch (tac_list->type) {
            case TAC_BEGINFUN:
                fprintf(asm_file, ".globl\t%s\n"
                        ".type\t%s, @function\n"
                        "%s:\n"
                        "\tpushq\t%%rbp\n"
                        "\tmovq\t%%rsp, %%rbp\n",
                        tac_list->res->string,
                        tac_list->res->string,
                        tac_list->res->string
                );
                break;
            case TAC_ENDFUN:
                fprintf(asm_file, "\tpopq\t%%rbp\n"
                        "\tret\n\n"
                       );
                break;
            default:
                break;
        }

        tac_list = tac_list->neigh;
    }
}

void generateAsm(const char *file_name, TacNode *tac_list) {
    FILE *file = fopen(file_name, "w");    
    write_read_only(file);
    fprintf(file, "\n");
    write_instructions(file, tac_list);
    fclose(file);
}
