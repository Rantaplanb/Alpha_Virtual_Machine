#ifndef AVM_EXEC_ARITHMETIC_H
#define AVM_EXEC_ARITHMETIC_H

#include "../memory/instruction.h"

#define execute_add execute_arithmetic
#define execute_sub execute_arithmetic
#define execute_mul execute_arithmetic
#define execute_div execute_arithmetic
#define execute_mod execute_arithmetic
#define execute_nop execute_arithmetic

void execute_assign(Instruction *);
void execute_arithmetic(Instruction *);

#endif  // AVM_EXEC_ARITHMETIC_H
