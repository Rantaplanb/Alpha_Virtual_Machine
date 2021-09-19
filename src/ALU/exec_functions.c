#include "exec_functions.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../ALU/dispatcher.h"
#include "../memory/function.h"
#include "../memory/stack.h"
#include "../memory/table.h"
#include "../state.h"
#include "../utilities.h"


void execute_call(Instruction *instr) {
    avm_memcell *func = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell *functor, *functor_key;
    if (executionFinished) return;
    assert(func);

    switch (func->type) {
        case userfunc_m:
            avm_callsaveenvironment();
            if (executionFinished) return;
            assert(func->data.funcVal < total_user_funcs);
            pc = user_funcs[func->data.funcVal].address;
            assert(pc < AVM_ENDING_PC);
            assert(code[pc].opcode == enterfunc_v);
            break;
        case string_m:
            avm_callsaveenvironment();
            if (executionFinished) return;
            avm_calllibfunc(func->data.strVal);
            break;
        case libfunc_m:
            avm_callsaveenvironment();
            if (executionFinished) return;
            avm_calllibfunc(func->data.libfuncVal);
            break;
        case table_m:
            functor_key = calloc(sizeof(avm_memcell), 1);
            functor_key->type = string_m;
            functor_key->data.strVal = strdup("()");
            functor = HashMap_get(func->data.tableVal->strIndexed, functor_key);
            if (functor && functor->type == userfunc_m) {
                avm_assign(&stack[top], func);
                ++totalActuals;
                avm_dec_top();
                avm_callsaveenvironment();
                if (executionFinished) return;
                pc = user_funcs[functor->data.funcVal].address;
                break;
            }
        default: {
            char *s = avm_tostring(func);
            avm_error("could not find function '%s'.", s);
            free(s);
        }
    }
}

void execute_funcenter(Instruction *instr) {
    avm_memcell *func = avm_translate_operand(&instr->result, &ax);
    if (executionFinished) return;
    assert(func);
    assert(pc == user_funcs[func->data.funcVal]
                     .address); /* Func address should match PC. */

    /* Callee actions here. */
    totalActuals = 0;
    UserFunc *funcInfo = avm_getfuncinfo(pc);
    topsp = top;
    top = top - funcInfo->local_size;
}

void execute_funcexit(Instruction *unused) {
    unsigned oldTop = top;
    top = avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET);
    pc = avm_get_envvalue(topsp + AVM_SAVEDPC_OFFSET);
    topsp = avm_get_envvalue(topsp + AVM_SAVEDTOPSP_OFFSET);
    while (++oldTop <= top) avm_memcellclear(&stack[oldTop]);
}

void execute_pusharg(Instruction *instr) {
    avm_memcell *arg = avm_translate_operand(&instr->arg1, &ax);
    if (executionFinished) return;
    assert(arg);

    /* This is actually stack[top] = arg, but we have to use avm_assign */
    avm_assign(&stack[top], arg);
    ++totalActuals;
    avm_dec_top();
}
