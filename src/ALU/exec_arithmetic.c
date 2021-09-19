#include "exec_arithmetic.h"

#include <assert.h>

#include "../memory/stack.h"
#include "../memory/table.h"
#include "../state.h"
#include "../utilities.h"
#include "dispatcher.h"

typedef double (*arithmetic_func_t)(double x, double y);

double add_impl(double x, double y) { return x + y; }

double sub_impl(double x, double y) { return x - y; }

double mul_impl(double x, double y) { return x * y; }

double div_impl(double x, double y) {
    if (y == 0) avm_error("division with 0.");
    return x / y;
}

double mod_impl(double x, double y) {
    if (y == 0) avm_error("modulo with 0.");
    return ((unsigned int)x) % ((unsigned int)y);
}

static arithmetic_func_t arithmeticFuncs[] = {add_impl, sub_impl, mul_impl,
                                              div_impl, mod_impl};


void execute_assign(Instruction *instr) {
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell *)0);
    avm_memcell *rv = avm_translate_operand(&instr->arg1, &ax);

    assert(lv && ((&stack[current_stack_size - 1] >= lv && lv > &stack[top]) ||
                  lv == &retval));
    if (executionFinished) return;
    avm_assign(lv, rv);
}

void execute_arithmetic(Instruction *instr) {
    avm_memcell *lv = avm_translate_operand(&instr->result, (avm_memcell *)0);
    avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);
    if (executionFinished) return;
    assert(lv && ((&stack[current_stack_size - 1] >= lv && lv > &stack[top]) ||
                  lv == &retval));
    assert(rv1 && rv2);

    if (rv1->type != number_m || rv2->type != number_m) {
        avm_error(
            "Can't execute arithmetic operations with non numeric arguments");
    } else {
        arithmetic_func_t op = arithmeticFuncs[instr->opcode - add_v];
        avm_memcellclear(lv);
        lv->type = number_m;
        lv->data.numVal = (*op)(rv1->data.numVal, rv2->data.numVal);
    }
}
