#include "memory.h"

#include <assert.h>
#include <stdarg.h>
#include <stdio.h>

#include "../ALU/dispatcher.h"
#include "../utilities.h"
#include "table.h"


// Source arrays
unsigned total_num_consts;
double *num_consts;
unsigned total_string_consts;
char **string_consts;
unsigned total_lib_funcs_used;
char **lib_funcs_used;
unsigned total_user_funcs;
UserFunc *user_funcs;
unsigned total_globals;

// Library Function map
HashMap *lib_map;

// Instructions
unsigned code_size;
Instruction *code;

// Registers
avm_memcell ax, bx, cx, retval;
unsigned pc = 0;


void avm_assign(avm_memcell *lv, avm_memcell *rv) {
    if (lv == rv) return;
    if (lv->type == table_m && rv->type == table_m &&
        lv->data.tableVal == rv->data.tableVal)
        return;
    if (rv->type == undef_m) avm_warning("Assigning from 'undef' content!\n");

    avm_memcellclear(lv);

    memcpy(lv, rv, sizeof(avm_memcell));

    if (lv->type == string_m)
        lv->data.strVal = strdup(rv->data.strVal);
    else if (lv->type == table_m)
        avm_tableincrefcounter(lv->data.tableVal);
}

double consts_getnumber(unsigned index) {
    assert(index < total_num_consts);
    return num_consts[index];
}

char *consts_getstring(unsigned index) {
    assert(index < total_string_consts);
    return string_consts[index];
}

char *libfuncs_getused(unsigned index) {
    assert(index < total_lib_funcs_used);
    return lib_funcs_used[index];
}
