#include "dispatcher.h"

#include <assert.h>

#include "../memory/memory.h"
#include "../memory/stack.h"
#include "../state.h"
#include "../utilities.h"

unsigned currLine = 0;


avm_memcell *avm_translate_operand(VMarg *arg, avm_memcell *reg) {
    switch (arg->type) {
        case global_a:
            return &stack[AVM_STACKSIZE - 1 - arg->val];
        case local_a:
            return &stack[topsp - arg->val];
        case formal_a:
            if (topsp + AVM_STACKENV_SIZE + 1 + arg->val >
                avm_get_envvalue(topsp + AVM_SAVEDTOP_OFFSET))
                avm_error("Argument not pushed in stack.");
            return &stack[topsp + AVM_STACKENV_SIZE + 1 + arg->val];
        case retval_a:
            return &retval;
        case number_a:
            reg->type = number_m;
            reg->data.numVal = consts_getnumber(arg->val);
            return reg;
        case string_a:
            reg->type = string_m;
            reg->data.strVal = strdup(consts_getstring(arg->val));
            return reg;
        case bool_a:
            reg->type = bool_m;
            reg->data.boolVal = arg->val;
            return reg;
        case nil_a:
            reg->type = nil_m;
            return reg;
        case userfunc_a:
            reg->type = userfunc_m;
            reg->data.funcVal = arg->val;
            return reg;
        case libfunc_a:
            reg->type = libfunc_m;
            reg->data.libfuncVal = libfuncs_getused(arg->val);
            return reg;
        default:
            assert(0);
    }
}

execute_func_t executeFuncs[] = {
    execute_assign,       execute_add,          execute_sub,
    execute_mul,          execute_div,          execute_mod,
    execute_jeq,          execute_jne,          execute_jle,
    execute_jge,          execute_jlt,          execute_jgt,
    execute_jump,         execute_call,         execute_pusharg,
    execute_funcenter,    execute_funcexit,     execute_newtable,
    execute_tablegetelem, execute_tablesetelem, execute_nop};


void execute_cycle(void) {
    if (executionFinished) return;
    if (pc == AVM_ENDING_PC) {
        executionFinished = true;
        return;
    }
    assert(pc < AVM_ENDING_PC);
    Instruction *instr = code + pc;
    assert(instr->opcode >= 0 && instr->opcode <= AVM_MAX_INSTRUCTIONS);
    if (instr->srcLine) currLine = instr->srcLine;
    unsigned oldPc = pc;
    (*executeFuncs[instr->opcode])(instr);
    if (pc == oldPc) ++pc;
}
