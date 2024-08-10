#include <stdio.h>
#include <string.h>

#include "hash.h"
#include "asm.h"

// Parte fixa

void write_print_formats(FILE *asm_file) {
    fprintf(asm_file, "_printint:\n"
            "\t.string\t\"%%ld\"\n"
            "_printfloat:\n"
            "\t.string\t\"%%f\"\n"
            "_printchar:\n"
            "\t.string\t\"%%c\"\n");
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
    write_parameters(asm_file, ast_tree);
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
        case SYMBOL_FUNCTION:
            name = table_entry->string;
            break;
        default:
            name = table_entry->id;
            break;
    }

    return name;
}

void load_compatible_operands_binary(FILE *asm_file, TacNode *node) {
    if (node->op1->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%ecx\n",
                get_asm_name(node->op1));
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%ecx\n",
                get_asm_name(node->op1));
    }

    if (node->op2->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%edx\n",
                get_asm_name(node->op2));
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%edx\n",
                get_asm_name(node->op2));
    }
}

void load_first_as_int(FILE *asm_file, TacNode *node) {
    if (node->op1->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%eax\n",
                get_asm_name(node->op1));
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1));
    }
}

void load_first_as_char(FILE *asm_file, TacNode *node) {
    if (node->op1->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovb\t_%s(%%rip), %%al\n",
                get_asm_name(node->op1)
        );
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1)
        );
    }
}

void load_second_as_int(FILE *asm_file, TacNode *node) {
    if (node->op2->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%eax\n",
                get_asm_name(node->op2));
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op2));
    }
}

void load_second_as_char(FILE *asm_file, TacNode *node) {
    if (node->op2->datatype == DATATYPE_CHAR) {
        fprintf(asm_file, "\tmovb\t_%s(%%rip), %%al\n",
                get_asm_name(node->op2)
        );
    } else {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op2)
        );
    }
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\taddl\t%%edx, %%ecx\n"
                "\tmovl\t%%ecx, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tsubl\t%%edx, %%ecx\n"
                "\tmovl\t%%ecx, _%s(%%rip)\n",
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
            load_compatible_operands_binary(asm_file, node);
            fprintf(asm_file, "\timull\t%%edx, %%ecx\n"
                    "\tmovl\t%%ecx, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tmovl\t%%ecx, %%eax\n"
                "\tmovl\t%%edx, %%ecx\n"
                "\tcltd\n"
                "\tidivl\t%%ecx\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                "\tsetl\t%%dl\n"
                "\tmovzbl\t%%dl, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                "\tsetg\t%%dl\n"
                "\tmovzbl\t%%dl, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
            fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                    "\tsetle\t%%dl\n"
                    "\tmovzbl\t%%dl, %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                "\tsetge\t%%dl\n"
                "\tmovzbl\t%%dl, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                "\tsete\t%%dl\n"
                "\tmovzbl\t%%dl, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
        load_compatible_operands_binary(asm_file, node);
        fprintf(asm_file, "\tcmpl\t%%edx, %%ecx\n"
                "\tsetne\t%%dl\n"
                "\tmovzbl\t%%dl, %%eax\n"
                "\tmovl\t%%eax, _%s(%%rip)\n",
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
            load_second_as_int(asm_file, node);
            fprintf(asm_file, "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovl\t(%%rdx,%%rax), %%eax\n"
                    "\tmovl\t%%eax, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            load_second_as_int(asm_file, node);
            fprintf(asm_file, "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovss\t(%%rdx,%%rax), %%xmm0\n"
                    "\tmovss\t%%xmm0, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            load_second_as_int(asm_file, node);
            fprintf(asm_file, "\tcltq\n"
                    "\tleaq\t_%s(%%rip), %%rdx\n"
                    "\tmovzbl\t(%%rax,%%rdx), %%eax\n"
                    "\tmovb\t%%al, _%s(%%rip)\n",
                    get_asm_name(node->op1),
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_call(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tcall\t_%s\n", get_asm_name(node->op1));
}

void write_move(FILE *asm_file, TacNode *node) {
    switch (node->res->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            load_first_as_int(asm_file, node);
            fprintf(asm_file, "\tmovl\t%%eax, _%s(%%rip)\n",
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
            load_first_as_char(asm_file, node);
            fprintf(asm_file, "\tmovb\t%%al, _%s(%%rip)\n",
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_arg(FILE *asm_file, TacNode *node) {
    write_move(asm_file, node);
}

void write_vecmove(FILE *asm_file, TacNode *node) {
    switch (node->res->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            load_first_as_int(asm_file, node);
            fprintf(asm_file, "\tmovl\t%%eax, %%edx\n");
            load_second_as_int(asm_file, node);
            fprintf(asm_file, "\tmovslq\t%%edx, %%rdx\n"
                    "\tleaq\t0(,%%rdx,4), %%rcx\n"
                    "\tleaq\t_%s(%%rip), %%rdx\n"
                    "\tmovl\t%%eax, (%%rcx,%%rdx)\n",
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            load_first_as_int(asm_file, node);
            fprintf(asm_file, "\tmovss\t_%s(%%rip), %%xmm0\n"
                    "\tcltq\n"
                    "\tleaq\t0(,%%rax,4), %%rdx\n"
                    "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovss\t%%xmm0, (%%rdx,%%rax)\n",
                    get_asm_name(node->op2),
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            load_second_as_char(asm_file, node);
            fprintf(asm_file, "\tmovzbl\t%%eax, %%edx\n");
            load_first_as_int(asm_file, node);
            fprintf(asm_file, "\tcltq\n"
                    "\tleaq\t_%s(%%rip), %%rcx\n"
                    "\tmovb\t%%dl, (%%rax,%%rcx)\n",
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_ret(FILE *asm_file, TacNode *node, HashEntry *func) {
    if (strcmp(func->string, "main") == 0) {
        load_first_as_int(asm_file, node);
        fprintf(asm_file, "\tpopq\t%%rbp\n");
    } else {
        switch (node->res->datatype) {
            case DATATYPE_INT:
            case DATATYPE_BOOL:
            case DATATYPE_REAL:
                load_first_as_int(asm_file, node);
                fprintf(asm_file, "\tmovl\t%%eax, _%s(%%rip)\n",
                        get_asm_name(node->res)
                );
                break;
            case DATATYPE_CHAR:
                load_first_as_char(asm_file, node);
                fprintf(asm_file, "\tmovb\t%%al, _%s(%%rip)\n",
                        get_asm_name(node->res)
                );
                break;
            default:
                break;
        }
        fprintf(asm_file, "\tleave\n");
    }

    fprintf(asm_file, "\tret\n");
}

void write_printint(FILE *asm_file, TacNode *node) {
    if (node->op1->datatype == DATATYPE_INT) {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1)
        );
    } else {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%eax\n",
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
    if (node->op1->datatype == DATATYPE_INT) {
        fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n",
                get_asm_name(node->op1)
        );
    } else {
        fprintf(asm_file, "\tmovzbl\t_%s(%%rip), %%eax\n"
                "\tmovsbl\t%%al, %%eax\n",
                get_asm_name(node->op1)
        );
    }
    
    fprintf(asm_file, "\tmovl\t%%eax, %%esi\n"
            "\tleaq\t_printchar(%%rip), %%rax\n"
            "\tmovq\t%%rax, %%rdi\n"
            "\tmovl\t$0, %%eax\n"
            "\tcall\tprintf@PLT\n"
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

void write_read(FILE *asm_file, TacNode *node) {
    switch (node->res->datatype) {
        case DATATYPE_INT:
        case DATATYPE_BOOL:
            fprintf(asm_file, "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rsi\n"
                    "\tleaq\t_printint(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\t__isoc99_scanf@PLT\n",
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_REAL:
            fprintf(asm_file, "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rsi\n"
                    "\tleaq\t_printfloat(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\t__isoc99_scanf@PLT\n",
                    get_asm_name(node->res)
            );
            break;
        case DATATYPE_CHAR:
            fprintf(asm_file, "\tleaq\t_%s(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rsi\n"
                    "\tleaq\t_printchar(%%rip), %%rax\n"
                    "\tmovq\t%%rax, %%rdi\n"
                    "\tmovl\t$0, %%eax\n"
                    "\tcall\t__isoc99_scanf@PLT\n",
                    get_asm_name(node->res)
            );
            break;
        default:
            break;
    }
}

void write_ifz(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tmovl\t_%s(%%rip), %%eax\n"
            "\ttestl\t%%eax, %%eax\n"
            "\tje\t_%s\n",
            get_asm_name(node->op1),
            get_asm_name(node->res)
    );
}

void write_label(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "_%s:\n", get_asm_name(node->res));
}

void write_jump(FILE *asm_file, TacNode *node) {
    fprintf(asm_file, "\tjmp\t_%s\n", get_asm_name(node->res));
}

void write_beginfun(FILE *asm_file, TacNode *node) {
    if (strcmp(node->res->string, "main") == 0) {
        fprintf(asm_file, ".globl\t%s\n"
                ".type\t%s, @function\n"
                "%s:\n",
                node->res->string,
                node->res->string,
                node->res->string
        );
    } else {
        fprintf(asm_file, ".globl\t_%s\n"
                ".type\t_%s, @function\n"
                "_%s:\n",
                node->res->string,
                node->res->string,
                node->res->string
        );
    }

    fprintf(asm_file, "\tpushq\t%%rbp\n"
                "\tmovq\t%%rsp, %%rbp\n"
    );
}

void write_endfun(FILE *asm_file, TacNode *node, HashEntry *current_func) {
    if (strcmp(node->res->string, "main") == 0) {
        fprintf(asm_file, "\tmovl\t$0, %%eax\n"
                "\tpopq\t%%rbp\n"
                "\tret\n"
        );
    }
}

void write_instructions(FILE *asm_file, TacNode *tac_list) {
    HashEntry *current_func = NULL;

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
            case TAC_CALL:
                write_call(asm_file, tac_list);
                break;
            case TAC_ARG:
                write_arg(asm_file, tac_list);
                break;
            case TAC_MOVE:
                write_move(asm_file, tac_list);
                break;
            case TAC_VECMOVE:
                write_vecmove(asm_file, tac_list);
                break;
            case TAC_RET:
                write_ret(asm_file, tac_list, current_func);
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
            case TAC_READ:
                write_read(asm_file, tac_list);
                break;
            case TAC_IFZ:
                write_ifz(asm_file, tac_list);
                break;
            case TAC_JUMP:
                write_jump(asm_file, tac_list);
                break;
            case TAC_LABEL:
                write_label(asm_file, tac_list);
                break;
            case TAC_BEGINFUN:
                current_func = tac_list->res;
                write_beginfun(asm_file, tac_list);
                break;
            case TAC_ENDFUN:
                current_func = NULL;
                write_endfun(asm_file, tac_list, current_func);
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
