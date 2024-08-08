#include <stdio.h>

#include "hash.h"
#include "asm.h"

// Parte fixa

void write_print_formats(FILE *asm_file) {
    fprintf(asm_file, "_printint:\n"
            "\t.string\t\"%%ld\"\n"
            "_printfloat:\n"
            "\t.string\t\"%%f\"\n");
}

void write_read_only(FILE *asm_file) {
    fprintf(asm_file, "## READ-ONLY\n"
            ".section\t.rodata\n");
    write_print_formats(asm_file);
    write_literals(asm_file);
}

void write_data_section(FILE *asm_file, AstNode *ast_tree) {
    fprintf(asm_file, "## VARIABLE DATA\n"
            ".section\t.data\n");
    write_var_decl(asm_file, ast_tree);
    write_vec_decl_def(asm_file, ast_tree);
}

void write_bss_section(FILE *asm_file, AstNode *ast_tree) {
    fprintf(asm_file, "## UNINITIALIZED DATA\n"
            ".bss\n");
    write_vec_decl(asm_file, ast_tree);
    write_temps(asm_file);
}

// Comandos

char *get_asm_name(HashEntry *table_entry) {
    char *name = NULL;

    switch (table_entry->type) {
        case SYMBOL_TEMP:
        case SYMBOL_LABEL:
        case SYMBOL_VARIABLE:
        case SYMBOL_VECTOR:
            name = table_entry->string;
            break;
        default:
            name = table_entry->id;
            break;
    }

    return name;
}

void write_add(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\taddss\t%%xmm1, %%xmm0\n"
                "\tmovss\t%%xmm0, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%ecx\n"
                "\tmovl\t%%edx, %%eax\n"
                "\taddl\t%%ecx, %%eax\n"
                "\tmovb\t%%al, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\taddl\t%%eax, %%edx\n"
                    "\tmovl\t%%edx, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_sub(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tsubss\t%%xmm1, %%xmm0\n"
                "\tmovss\t%%xmm0, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%ecx\n"
                "\tmovl\t%%edx, %%eax\n"
                "\tsubl\t%%ecx, %%eax\n"
                "\tmovb\t%%al, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tsubl\t%%eax, %%edx\n"
                    "\tmovl\t%%edx, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_mul(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmulss\t%%xmm1, %%xmm0\n"
                "\tmovss\t%%xmm0, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%ecx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovl\t%%eax, %%edx\n"
                "\tmovl\t%%ecx, %%eax\n"
                "\timull\t%%edx, %%eax\n"
                "\tmovb\t%%al, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\timull\t%%edx, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_div(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tdivss\t%%xmm1, %%xmm0\n"
                "\tmovss\t%%xmm0, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%eax\n"
                "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovsbl\t%%dl, %%esi\n"
                "\tcltd"
                "\tidivl\t%%esi\n"
                "\tmovb\t%%al, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tmovl\t_%s(%%rip), %%ecx\n"
                    "\tcltd\n"
                    "\tidivl\t%%ecx\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_less(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tcomiss\t%%xmm1, %%xmm0\n"
                "\tseta\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsetl\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsetl\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_greater(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tcomiss\t%%xmm1, %%xmm0\n"
                "\tseta\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsetg\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsetg\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_le(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tcomiss\t%%xmm1, %%xmm0\n"
                "\tsetnb\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsetle\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsetle\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_ge(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tcomiss\t%%xmm1, %%xmm0\n"
                "\tsetnb\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsetge\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsetge\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_eq(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tucomiss\t%%xmm1, %%xmm0\n"
                "\tsetnp\t%%al\n"
                "\tmovl\t$0, %%edx\n"
                "\tucomiss\t%%xmm1, %%xmm0\n"
                "\tcmovne\t%%edx, %%eax\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsete\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsete\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_dif(FILE *asm_file, TacNode *node) {
    if (node->res->datatype == DATATYPE_REAL) {
        fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                "\tmovss\t_%s(%%rip), %%xmm1\n"
                "\tucomiss\t%%xmm1, %%xmm0\n"
                "\tsetp\t%%al\n"
                "\tmovl\t$1, %%edx\n"
                "\tucomiss\t%%xmm1, %%xmm0\n"
                "\tcmovne\t%%edx, %%eax\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else if (node->res->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%edx\n"
                "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tcmpb\t%%al, %%dl\n"
                "\tsetne\t%%al\n"
                "\tmovzbl\t%%al, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
                get_asm_name(node->op1),
                get_asm_name(node->op2),
                get_asm_name(node->res)
        );
    } else {
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcmpl\t%%eax, %%edx\n"
                    "\tsetne\t%%al\n"
                    "\tmovzbl\t%%al, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
    }
}

void write_and(FILE *asm_file, TacNode *node) {
    HashEntry *label_1 = makeLabel();
    HashEntry *label_2 = makeLabel();

    fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tje\t_%s\n"
            "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tje\t_%s\n"
            "\tmovl\t$1, %%eax\n"
            "\tjmp\t_%s\n"
            "_%s:\n"
            "\tmovl\t$0, %%eax\n"
            "_%s:\n"
            "\tmovl\t%%eax, _%s(%%rip)\n",
            get_asm_name(node->op1),
            get_asm_name(label_1),
            get_asm_name(node->op2),
            get_asm_name(label_1),
            get_asm_name(label_2),
            get_asm_name(label_1),
            get_asm_name(label_2),
            get_asm_name(node->res)
    );
}

void write_or(FILE *asm_file, TacNode *node) {
    HashEntry *label_1 = makeLabel();
    HashEntry *label_2 = makeLabel();
    HashEntry *label_3 = makeLabel();

    fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tjne\t_%s\n"
            "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tje\t_%s\n"
            "_%s:\n"
            "\tmovl\t$1, %%eax\n"
            "\tjmp\t_%s\n"
            "_%s:\n"
            "\tmovl\t$0, %%eax\n"
            "_%s:\n"
            "\tmovl\t%%eax, _%s(%%rip)\n",
            get_asm_name(node->op1),
            get_asm_name(label_1),
            get_asm_name(node->op2),
            get_asm_name(label_2),
            get_asm_name(label_1),
            get_asm_name(label_3),
            get_asm_name(label_2),
            get_asm_name(label_3),
            get_asm_name(node->res)
    );
}

void write_not(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tsete\t%%al\n"
            "\tmovzbl\t%%al, %%eax\n"
            "\tmovl\t%%eax, _%s(%%rip)\n",
            get_asm_name(node->op1),
            get_asm_name(node->res)
    );
}

void write_vec_read(FILE *asm_file, TacNode *node) {
    switch (node->op1->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovl\t(%%rdx,%%rax), %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op2),
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovss\t(%%rdx,%%rax), %%xmm0\n"
                    "\tmovss\t%%xmm0, _%s(%%rip)\n",
                    get_asm_name(node->op2),
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tcltq\n"
                    "\tleaq\t_%s(%%rip), %%rdx\n"
                    "\tmovzbl\t(%%rax,%%rdx), %%eax\n"
                    "\tmovb\t%%al, _%s(%%rip)\n",
                    get_asm_name(node->op2),
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_move(FILE *asm_file, TacNode *node) {
    switch (node->res->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                    "\tmovss\t%%xmm0, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                    "\tmovb\t%%al, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_vecmove(FILE *asm_file, TacNode *node) {
    switch (node->res->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n"
                    "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tmovslq\t%%edx, %%rdx\n"
                    "\tleaq\t0(,%%rdx,4), %%rcx\n"
                    "\tleaq\t_%s(%%rip), %%rdx\n"
                    "\tmovl\t%%eax, (%%rcx,%%rdx)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tmovss\t_%s(%%rip), %%xmm0\n"
                    "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovss\t%%xmm0, (%%rdx,%%rax)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
                    "\tmovzbl\t_%s(%%rip), %%edx\n"
                    "\tcltq\n"
                    "\tleaq\t_%s(%%rip), %%rcx\n"
                    "\tmovb\t%%dl, (%%rax,%%rcx)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_printint(FILE *asm_file, TacNode *node) {
    if (node->op1->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzx\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1)
        );
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1)
        );
    }

    fprintf(asm_file, "\tmovl\t%%eax, %%esi\n"
            "\tleaq\t_printint(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$0, %%eax\n"
            "\tcall\tprintf@PLT\n"
    );
}

void write_printfloat(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
            "\tpxor\t%%xmm1, %%xmm1\n"
            "\tcvtss2sd\t%%xmm0, %%xmm1\n"
            "\tmovq\t%%xmm1, %%rax\n"
            "\tmovq\t%%rax, %%xmm0\n"
            "\tleaq\t_printfloat(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$1, %%eax\n"
            "\tcall\tprintf@PLT\n",
            get_asm_name(node->op1)
    );
}

void write_printchar(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
            "\tmovsbl\t%%al, %%eax\n"
            "\tmovl\t%%eax, %%edi\n"
            "\tcall\tputchar@PLT\n",
            get_asm_name(node->op1)
    );
}

void write_printstring(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tleaq\t_%s(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$0, %%eax\n"
            "\tcall\tprintf@PLT\n",
            get_asm_name(node->op1)
    );
}

void write_instructions(FILE *asm_file, TacNode *tac_list) {
    fprintf(asm_file, "## INSTRUCTIONS\n"
            ".text\n");
    
    while (tac_list != NULL) {
        switch (tac_list->type) {
            case TAC_ADD:
                write_add(asm_file, tac_list);
                break;
            case TAC_SUB:
                write_sub(asm_file, tac_list);
                break;
            case TAC_MUL:
                write_mul(asm_file, tac_list);
                break;
            case TAC_DIV:
                write_div(asm_file, tac_list);
                break;
            case TAC_LESS:
                write_less(asm_file, tac_list);
                break;
            case TAC_GREATER:
                write_greater(asm_file, tac_list);
                break;
            case TAC_LE:
                write_le(asm_file, tac_list);
                break;
            case TAC_GE:
                write_ge(asm_file, tac_list);
                break;
            case TAC_EQ:
                write_eq(asm_file, tac_list);
                break;
            case TAC_DIF:
                write_dif(asm_file, tac_list);
                break;
            case TAC_AND:
                write_and(asm_file, tac_list);
                break;
            case TAC_OR:
                write_or(asm_file, tac_list);
                break;
            case TAC_NOT:
                write_not(asm_file, tac_list);
                break;
            case TAC_VECREAD:
                write_vec_read(asm_file, tac_list);
                break;
            case TAC_MOVE:
                write_move(asm_file, tac_list);
                break;
            case TAC_VECMOVE:
                write_vecmove(asm_file, tac_list);
                break;
            case TAC_PRINTINT:
            case TAC_PRINTBOOL:
                write_printint(asm_file, tac_list);
                break;
            case TAC_PRINTFLOAT:
                write_printfloat(asm_file, tac_list);
                break;
            case TAC_PRINTCHAR:
                write_printchar(asm_file, tac_list);
                break;
            case TAC_PRINTSTRING:
                write_printstring(asm_file, tac_list);
                break;
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

void generateAsm(const char *file_name, AstNode *ast_tree, TacNode *tac_list) {
    FILE *file = fopen(file_name, "w");    
    write_read_only(file);
    fprintf(file, "\n");
    write_data_section(file, ast_tree);
    fprintf(file, "\n");
    write_bss_section(file, ast_tree);
    fprintf(file, "\n");
    write_instructions(file, tac_list);
    fclose(file);
}
