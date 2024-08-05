#include <stdio.h>

#include "hash.h"
#include "asm.h"

void write_print_formats(FILE *asm_file) {
    fprintf(asm_file, "printint:\n"
            "\t.string\t\"%%ld\"\n"
            "printfloat:\n"
            "\t.string\t\"%%f\"\n");
}

void write_read_only(FILE *asm_file) {
    fprintf(asm_file, "## READ-ONLY\n"
            ".section\t.rodata\n");
    write_print_formats(asm_file);
    write_literals(asm_file);
}

void write_printint(FILE *asm_file) {
    fprintf(asm_file, "\tmovq\t%%rax, %%rsi\n"
            "\tleaq\tprintint(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$0, %%eax\n"
            "\tcall\tprintf@PLT\n"
        );
}

void write_printfloat(FILE *asm_file) {
    fprintf(asm_file, "\tmovq\t%%rax, %%xmm0\n"
            "\tleaq\tprintfloat(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$1, %%eax\n"
            "\tcall\tprintf@PLT\n"
    );
}

void write_printchar(FILE *asm_file) {
    fprintf(asm_file, "\tmovl\t%%eax, %%edi\n"
            "\tcall\tputchar@PLT\n"
            "\tmovl\t$0, %%eax\n"
    );
}

void write_printstring(FILE *asm_file) {
    fprintf(asm_file, "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$0, %%eax\n"
            "\tcall\tprintf@PLT\n"
    );
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
            case TAC_PRINTINT:
            case TAC_PRINTBOOL:
                write_printint(asm_file);
                break;
            case TAC_PRINTFLOAT:
                write_printfloat(asm_file);
                break;
            case TAC_PRINTCHAR:
                write_printchar(asm_file);
                break;
            case TAC_PRINTSTRING:
                write_printstring(asm_file);
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
