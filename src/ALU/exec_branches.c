#include "exec_branches.h"

#include <assert.h>

#include "../memory/memory.h"
#include "../state.h"
#include "../utilities.h"
#include "dispatcher.h"


typedef bool (*equality_check_t)(avm_memcell *rv1, avm_memcell *rv2);
typedef bool (*comparison_func_t)(double x, double y);

bool equal_number(avm_memcell *rv1, avm_memcell *rv2) {
    assert(rv1->type == number_m && rv2->type == number_m);
    return rv1->data.numVal == rv2->data.numVal;
}

bool equal_string(avm_memcell *rv1, avm_memcell *rv2) {
    assert(rv1->type == string_m && rv2->type == string_m);
    return strcmp(rv1->data.strVal, rv2->data.strVal) ? false : true;
}

bool equal_table(avm_memcell *rv1, avm_memcell *rv2) {
    assert(rv1->type == table_m && rv2->type == table_m);
    return rv1->data.tableVal == rv2->data.tableVal;
}

bool equal_userfunctions(avm_memcell *rv1, avm_memcell *rv2) {
    assert(rv1->type == userfunc_m && rv2->type == userfunc_m);
    return rv1->data.funcVal == rv2->data.funcVal;
}
bool equal_libfunctions(avm_memcell *rv1, avm_memcell *rv2) {
    assert(rv1->type == libfunc_m && rv2->type == libfunc_m);
    return strcmp(rv1->data.libfuncVal, rv2->data.libfuncVal) ? false : true;
}

bool JGE_impl(double x, double y) { return x >= y ? true : false; }

bool JGT_impl(double x, double y) { return x > y ? true : false; }

bool JLE_impl(double x, double y) { return x <= y ? true : false; }

bool JLT_impl(double x, double y) { return x < y ? true : false; }

static comparison_func_t comparisonFuncs[] = {JLE_impl, JGE_impl, JLT_impl,
                                              JGT_impl};
static equality_check_t equalityCheck[] = {
    equal_number,        equal_string,       0, equal_table,
    equal_userfunctions, equal_libfunctions, 0, 0};

void execute_jeq(Instruction *instr) {
    assert(instr->result.type == label_a);
    avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);
    if (executionFinished) return;
    unsigned char result = 0;

    if (rv1->type == undef_m || rv2->type == undef_m)
        avm_error("'undef' involved in equality.");
    else if (rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type == nil_m && rv2->type == nil_m;
    else if (rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if (rv1->type != rv2->type)
        avm_error("%s == %s is illegal.", typeStrings[rv1->type],
                  typeStrings[rv2->type]);
    else {
        result = (*(equalityCheck[rv1->type]))(rv1, rv2);
    }

    if (!executionFinished && result) pc = instr->result.val;
}

void execute_jne(Instruction *instr) {
    assert(instr->result.type == label_a);
    avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);
    if (executionFinished) return;
    unsigned char result = 0;

    if (rv1->type == undef_m || rv2->type == undef_m)
        avm_error("'undef' involved in inequality.");
    else if (rv1->type == nil_m || rv2->type == nil_m)
        result = rv1->type == nil_m && rv2->type == nil_m;
    else if (rv1->type == bool_m || rv2->type == bool_m)
        result = (avm_tobool(rv1) == avm_tobool(rv2));
    else if (rv1->type != rv2->type)
        avm_error("%s == %s is illegal.", typeStrings[rv1->type],
                  typeStrings[rv2->type]);
    else {
        result = (*(equalityCheck[rv1->type]))(rv1, rv2);
    }

    if (!executionFinished && !result) pc = instr->result.val;
}

void comparison_dispatcher(Instruction *instr) {
    avm_memcell *rv1 = avm_translate_operand(&instr->arg1, &ax);
    avm_memcell *rv2 = avm_translate_operand(&instr->arg2, &bx);
    if (executionFinished) return;
    bool result = false;

    if (rv1->type != number_m || rv2->type != number_m) {
        avm_error("for comparisons, both arguments must be numbers.");
        return;
    }
    result = (*(comparisonFuncs[instr->opcode - if_lesseq_v]))(
        rv1->data.numVal, rv2->data.numVal);

    if (!executionFinished && result) pc = instr->result.val;
    return;
}

void execute_jle(Instruction *instr) { comparison_dispatcher(instr); }

void execute_jge(Instruction *instr) { comparison_dispatcher(instr); }

void execute_jlt(Instruction *instr) { comparison_dispatcher(instr); }

void execute_jgt(Instruction *instr) { comparison_dispatcher(instr); }

void execute_jump(Instruction *instr) {
    assert(instr->result.type == label_a);
    pc = instr->result.val;
    return;
}
