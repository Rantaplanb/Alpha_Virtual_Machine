#ifndef AVM_MEMORY_H
#define AVM_MEMORY_H

#include <string.h>

#include "../libs/hash_map.h"
#include "function.h"
#include "instruction.h"

#ifndef AVM_STACKENV_SIZE
#define AVM_STACKENV_SIZE 4
#endif

#ifndef AVM_STACKSIZE
#define AVM_NUMACTUALS_OFFSET 4
#define AVM_SAVEDPC_OFFSET 3
#define AVM_SAVEDTOP_OFFSET 2
#define AVM_SAVEDTOPSP_OFFSET 1
#define AVM_STACKSIZE 4096
#define AVM_WIPEOUT(m) memset(&(m), 0, sizeof(m))
#define AVM_ENDING_PC code_size
#endif

typedef enum {
    number_m = 0,
    string_m,
    bool_m,
    table_m,
    userfunc_m,
    libfunc_m,
    nil_m,
    undef_m
} avm_memcell_t;

typedef struct avm_table avm_table;

typedef struct {
    avm_memcell_t type;
    union {
        double numVal;
        char *strVal;
        unsigned char boolVal;
        avm_table *tableVal;
        unsigned int funcVal;
        char *libfuncVal;
    } data;
} avm_memcell;

typedef void (*library_func_t)(void);

// Source arrays
extern unsigned total_num_consts;
extern double *num_consts;
extern unsigned total_string_consts;
extern char **string_consts;
extern unsigned total_lib_funcs_used;
extern char **lib_funcs_used;
extern unsigned total_user_funcs;
extern UserFunc *user_funcs;
extern unsigned total_globals;

// Lib func map
extern HashMap *lib_map;

// Instructions
extern unsigned code_size;
extern Instruction *code;

// Registers
extern avm_memcell ax, bx, cx, retval;
extern unsigned pc;


void avm_assign(avm_memcell *lv, avm_memcell *rv);

double consts_getnumber(unsigned index);
char *consts_getstring(unsigned index);
char *libfuncs_getused(unsigned index);

#endif
