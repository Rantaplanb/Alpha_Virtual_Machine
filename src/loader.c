#include "loader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "memory/memory.h"


static char *bin_read_string(FILE *file) {
    char buffer[2048];
    size_t i = 0;
    char c;
    do {
        fread(&c, sizeof(char), 1, file);
        buffer[i++] = c;
    } while (c != '\0' && i < 2048);

    if (i == 2048) {
        fprintf(stderr, "Source code has a string too long for loading!\n");
        exit(1);
    }

    return strdup(buffer);
}

void load_abc(FILE *abc_file, char **string_consts[], double *num_consts[],
              UserFunc *funcs_used[], char **lib_funcs_used[],
              Instruction *instructions[]) {
    unsigned i;
    unsigned long long magic_number;

    fread(&magic_number, sizeof(magic_number), 1, abc_file);
    if (magic_number != 253876718360) {
        fprintf(stderr, "Invalid binary file.\n");
        exit(1);
    }

    // Const strings
    fread(&total_string_consts, sizeof(total_string_consts), 1, abc_file);
    *string_consts = calloc(sizeof(char *), total_string_consts);
    for (i = 0; i < total_string_consts; ++i)
        (*string_consts)[i] = bin_read_string(abc_file);

    // Const numbers
    fread(&total_num_consts, sizeof(total_num_consts), 1, abc_file);
    *num_consts = calloc(sizeof(double), total_num_consts);
    fread(*num_consts, sizeof(double), total_num_consts, abc_file);

    // User functions
    fread(&total_user_funcs, sizeof(total_user_funcs), 1, abc_file);
    *funcs_used = calloc(sizeof(UserFunc), total_user_funcs);
    int address, local_size;
    for (i = 0; i < total_user_funcs; ++i) {
        fread(&address, sizeof(int), 1, abc_file);
        fread(&local_size, sizeof(int), 1, abc_file);
        (*funcs_used)[i].address = address;
        (*funcs_used)[i].local_size = local_size;
        (*funcs_used)[i].id = bin_read_string(abc_file);
    }

    // Library functions
    fread(&total_lib_funcs_used, sizeof(total_lib_funcs_used), 1, abc_file);
    *lib_funcs_used = calloc(sizeof(char *), total_lib_funcs_used);
    for (i = 0; i < total_lib_funcs_used; ++i)
        (*lib_funcs_used)[i] = bin_read_string(abc_file);

    // Instructions
    fread(&code_size, sizeof(code_size), 1, abc_file);
    *instructions = calloc(sizeof(Instruction), code_size);
    total_globals = 0;
    for (i = 0; i < code_size; ++i)
        fread(&(*instructions)[i].srcLine, sizeof(unsigned), 1, abc_file);
    for (i = 0; i < code_size; ++i) {
        fread(&(*instructions)[i].opcode, sizeof(char), 1, abc_file);
        fread(&(*instructions)[i].result.type, sizeof(char), 1, abc_file);
        fread(&(*instructions)[i].result.val, sizeof(unsigned), 1, abc_file);
        fread(&(*instructions)[i].arg1.type, sizeof(char), 1, abc_file);
        fread(&(*instructions)[i].arg1.val, sizeof(unsigned), 1, abc_file);
        fread(&(*instructions)[i].arg2.type, sizeof(char), 1, abc_file);
        fread(&(*instructions)[i].arg2.val, sizeof(unsigned), 1, abc_file);
        if ((*instructions)[i].result.type == global_a &&
            (*instructions)[i].result.val + 1 > total_globals)
            total_globals = (*instructions)[i].result.val + 1;
        if ((*instructions)[i].arg1.type == global_a &&
            (*instructions)[i].arg1.val + 1 > total_globals)
            total_globals = (*instructions)[i].arg1.val + 1;
        if ((*instructions)[i].arg2.type == global_a &&
            (*instructions)[i].arg2.val + 1 > total_globals)
            total_globals = (*instructions)[i].arg2.val + 1;
    }

    return;
}
