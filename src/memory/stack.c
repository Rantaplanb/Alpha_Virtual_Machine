#include "stack.h"

#include <assert.h>

#include "../ALU/exec_functions.h"
#include "../libs/hash_map.h"
#include "../state.h"
#include "../std_lib/library_functions.h"
#include "../utilities.h"

avm_memcell stack[AVM_STACKSIZE];
unsigned current_stack_size = 0;
unsigned top, topsp;
unsigned totalActuals = 0;


static void avm_initstack(void) {
    for (unsigned int i = 0; i < AVM_STACKSIZE; ++i) {
        AVM_WIPEOUT(stack[i]);
        stack[i].type = undef_m;
    }
    topsp = AVM_STACKSIZE - 1;
    top = topsp - total_globals;
}

void avm_initialize(void) {
    avm_initstack();

    lib_map = HashMap_new(12, compare_strings, hash_string);
    avm_registerlibfunc("print", libfunc_print);
    avm_registerlibfunc("typeof", libfunc_typeof);
    avm_registerlibfunc("totalarguments", libfunc_totalarguments);
    avm_registerlibfunc("objecttotalmembers", libfunc_objecttotalmembers);
    avm_registerlibfunc("argument", libfunc_argument);
    avm_registerlibfunc("input", libfunc_input);
    avm_registerlibfunc("objectmemberkeys", libfunc_objectmemberkeys);
    avm_registerlibfunc("objectcopy", libfunc_objectcopy);
    avm_registerlibfunc("strtonum", libfunc_strtonum);
    avm_registerlibfunc("sqrt", libfunc_sqrt);
    avm_registerlibfunc("sin", libfunc_sin);
    avm_registerlibfunc("cos", libfunc_cos);
}

void avm_dec_top(void) {
    if (!top)
        avm_error("stack overflow.");
    else
        --top;
}

void avm_push_envvalue(unsigned int val) {
    stack[top].type = number_m;
    stack[top].data.numVal = val;
    avm_dec_top();
}

unsigned avm_get_envvalue(unsigned i) {
    assert(stack[i].type == number_m);
    unsigned val = (unsigned)stack[i].data.numVal;
    assert(stack[i].data.numVal == ((double)val));
    return val;
}

void avm_callsaveenvironment(void) {
    avm_push_envvalue(totalActuals);
    if (executionFinished) return;
    avm_push_envvalue(pc + 1);
    if (executionFinished) return;
    avm_push_envvalue(top + totalActuals + 2);
    if (executionFinished) return;
    avm_push_envvalue(topsp);
}


void avm_calllibfunc(char *func_name) {
    library_func_t f = avm_getlibraryfunc(func_name);
    if (!f) {
        avm_error("unsupported library function %s called.", func_name);
    } else {
        topsp = top; /* Enter function sequence. No stack locals yet */
        totalActuals = 0;
        (*f)();
        if (!executionFinished) execute_funcexit((Instruction *)0);
    }
}

unsigned int avm_totalactuals(void) {
    return avm_get_envvalue(topsp + AVM_NUMACTUALS_OFFSET);
}

avm_memcell *avm_getactual(unsigned int i) {
    assert(i < avm_totalactuals());
    return &stack[topsp + AVM_STACKENV_SIZE + 1 + i];
}
