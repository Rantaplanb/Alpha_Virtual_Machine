#include "ALU/dispatcher.h"
#include "loader.h"
#include "memory/memory.h"
#include "memory/stack.h"
#include "state.h"


int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "\nUsage: avm <byte_code_file>\n\n");
        return (1);
    }

    FILE *abc_file = fopen(argv[1], "rb");

    if (!abc_file) {
        fprintf(stderr, "\nCould not locate file %s!\n\n", argv[1]);
        return (1);
    }

    load_abc(abc_file, &string_consts, &num_consts, &user_funcs,
             &lib_funcs_used, &code);

    avm_initialize();

    while (!executionFinished) execute_cycle();

    return 0;
}
