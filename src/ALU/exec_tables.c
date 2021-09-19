#include "exec_tables.h"

#include <assert.h>
#include <stdlib.h>

#include "../memory/memory.h"
#include "../memory/stack.h"
#include "../memory/table.h"
#include "../state.h"
#include "../utilities.h"
#include "dispatcher.h"

void execute_newtable(Instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->arg1, (avm_memcell*)0);
    assert(lv && ((&stack[current_stack_size - 1] >= lv && lv > &stack[top]) ||
                  lv == &retval));

    avm_memcellclear(lv);

    lv->type = table_m;
    lv->data.tableVal = avm_tablenew();
    avm_tableincrefcounter(lv->data.tableVal);
}

void execute_tablegetelem(Instruction* instr) {
    avm_memcell* lv = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* t = avm_translate_operand(&instr->arg1, (avm_memcell*)0);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);
    if (executionFinished) return;

    assert(lv && ((&stack[current_stack_size - 1] >= lv && lv > &stack[top]) ||
                  lv == &retval));
    assert(t && &stack[current_stack_size - 1] >= t && t > &stack[top]);
    assert(i);

    avm_memcellclear(lv);
    lv->type = nil_m;

    if (t->type != table_m)
        avm_error("illegal use of type %s as table.", typeStrings[t->type]);
    else {
        avm_memcell* content = avm_tablegetelem(t->data.tableVal, i);
        if (content)
            avm_assign(lv, content);
        else {
            char* ts = avm_tostring(t);
            char* is = avm_tostring(i);
            avm_warning("index %s of %s not found.", is, ts);
            free(ts);
            free(is);
        }
    }
}

void execute_tablesetelem(Instruction* instr) {
    avm_memcell* t = avm_translate_operand(&instr->result, (avm_memcell*)0);
    avm_memcell* i = avm_translate_operand(&instr->arg2, &ax);
    avm_memcell* c = avm_translate_operand(&instr->arg1, &bx);
    if (executionFinished) return;
    assert(t && &stack[current_stack_size - 1] >= t && t > &stack[top]);
    assert(i && c);

    if (t->type != table_m)
        avm_error("illegal use of type %s as table.\n", typeStrings[t->type]);
    else
        avm_tablesetelem(t->data.tableVal, i, c);
}
