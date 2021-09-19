#ifndef AVM_DISPATCHER_H
#define AVM_DISPATCHER_H

#define AVM_MAX_INSTRUCTIONS (unsigned)nop_v

#include "../memory/instruction.h"
#include "exec_arithmetic.h"
#include "exec_branches.h"
#include "exec_functions.h"
#include "exec_tables.h"


typedef void (*execute_func_t)(Instruction *);

avm_memcell *avm_translate_operand(VMarg *arg, avm_memcell *reg);

void execute_cycle(void);

#endif  // AVM_DISPATCHER_H
